##############################################
#
# Module for connecting SHRDZM devices. (see http://shrdzm.com)
#
# written 2020 by Erich O. Pintar
#
##############################################
package main;

use strict;
use warnings;
use DevIo;


my %SHRDZM_gets = (
	"version"	=> ""
);

my %SHRDZM_sets = (
    "RCSend" => "",
    "reset" => ":noArg",
    "pair" => ":60,120,180"
);

my @topics = (
    "state",
	"update",
    "paired",
    "RCData",
    "IP",
    "version",
    "gatewayaddress",
	"+/config",
	"+/sensor",
	"+/sensors",
	"+/init",
	"+/param",
	"+/version",
);


sub SHRDZM_Initialize($) {
	my $hash = shift @_;
	
	require "$main::attr{global}{modpath}/FHEM/00_MQTT.pm";	
	require "$main::attr{global}{modpath}/FHEM/DevIo.pm";	

    $hash->{DefFn}      = "SHRDZM::DEVICE::Define";
	$hash->{ReadFn}  	= "SHRDZM::DEVICE::Read";	
    $hash->{WriteFn}    = "SHRDZM::DEVICE::Write";
	$hash->{UndefFn} 	= "SHRDZM::DEVICE::Undefine";
    $hash->{SetFn}      = "SHRDZM::DEVICE::Set";
	$hash->{AttrFn} 	= "SHRDZM::DEVICE::Attr";
	$hash->{ReadyFn} 	= "SHRDZM::DEVICE::Ready";
    $hash->{AttrList} 	= "IODev qos retain publishSet publishSet_.* subscribeReading_.* autoSubscribeReadings " . $main::readingFnAttributes;
    $hash->{OnMessageFn} = "SHRDZM::DEVICE::onmessage";

	$hash->{Clients} = "SHRDZMDevice";
  
	$hash->{MatchList} = { "1:SHRDZMDevice" => "\\S{12}\\s\\S+\\s{1}\\S+\:{1}\\S+" };  
  
    main::LoadModule("MQTT");
    main::LoadModule("MQTT_DEVICE");
    main::LoadModule("DevIo");
}

package SHRDZM::DEVICE;

use strict;
use warnings;
use POSIX;
use SetExtensions;
use GPUtils qw(:all);
use JSON;
use lib ('./FHEM/lib', './lib');
use Time::Local;
use Encode;
use Encode qw/from_to/;
use URI::Escape;
use DevIo;
use Net::MQTT::Constants;

BEGIN {
    MQTT->import(qw(:all));

    GP_Import(qw(
		DevIo_CloseDev
		DevIo_IsOpen
		DevIo_OpenDev
		DevIo_SimpleRead
		DevIo_SimpleWrite
		CommandDefine
        CommandDeleteReading
        CommandAttr
        readingsSingleUpdate
        readingsBulkUpdate
        readingsBeginUpdate
        readingsEndUpdate
        Log3
        SetExtensions
        SetExtensionsCancel
        fhem
        defs
		Dispatch
        AttrVal
        ReadingsVal
    ))
};

sub Define() 
{
    my ($hash, $def) = @_;
    my @args = split("[ \t]+", $def);

    return "Invalid number of arguments: define <name> SHRDZM <gatewayID> [<devicename[\@baudrate]>]]" if (int(@args) < 1);
		
	my ($name, $type, $topic) = @args;

	my $len = @args;
	
	if($len =~ 4) # serial connection
	{
		Log3($hash->{NAME}, 5, "NEW serial device : " . $args[3]);
		$hash->{Protocol}= "serial";
		$hash->{DeviceName}= $args[3];
		my $ret = undef;
		
		DevIo_CloseDev($hash) if(DevIo_IsOpen($hash)); 
		
		$ret = DevIo_OpenDev($hash, 0, undef);
		return $ret;
	}
	else
	{
		if (defined($topic)) 
		{

			$hash->{TOPIC} = $topic;
			$hash->{MODULE_VERSION} = "0.6";
			$hash->{READY} = 0;
			$hash->{Protocol}= "MQTT";

			$hash->{FULL_TOPIC} = "SHRDZM/".$topic."/";

			$hash->{TYPE} = 'MQTT_DEVICE';
			my $ret = MQTT::Client_Define($hash, $def);
			$hash->{TYPE} = $type;
			
			# Subscribe Readings
			foreach (@topics) 
			{
				my $newTopic = SHRDZM::DEVICE::GetTopicFor($hash, $_);
				my ($mqos, $mretain, $mtopic, $mvalue, $mcmd) = MQTT::parsePublishCmdStr($newTopic);
				MQTT::client_subscribe_topic($hash, $mtopic, $mqos, $mretain);

				Log3($hash->{NAME}, 5, "automatically subscribed to topic: " . $newTopic);
			}


			$hash->{READY} = 1;		
			
			return $ret;
		}
		else 
		{
			return "Topic missing";
		}
	}
};

sub Write ($$)
{
	my ( $hash, @arguments) = @_;

	if($hash->{Protocol} =~ "serial")
	{
		DevIo_SimpleWrite($hash, "\$set ".join(" ", @arguments)."\n", 2);
	}
	else
	{
		my $topic = $hash->{FULL_TOPIC} . "config/set";	
		my $retain = $hash->{".retain"}->{'*'};
		my $qos = $hash->{".qos"}->{'*'};	

		my $msgid = send_publish($hash->{IODev}, topic => $topic, message => join(" ", @arguments), qos => $qos, retain => $retain);
	}
}

sub Ready($)
{
  my ($hash) = @_;
  
  # try to reopen the connection in case the connection is lost
  return DevIo_OpenDev($hash, 1, undef);   
}
sub ParseMessage($$) # from serial
{
	my ($hash, $msg) = @_;
	my $name = $hash->{NAME};
	
	my @params = split('\$', $msg);
	my $len = @params;
	Log3 $name, 5, "params = ".join(" ", @params);
	
	if($len =~ 3)
	{
		if(length($params[0]) =~ 7)
		{
			if((substr($params[0], 4, 1) =~ '\[') && substr($params[0], 6, 1) =~ '\]')
			{
				if(substr($params[0], 5, 1) =~ "P")
				{
					Log3 $name, 5, "--- Pairing called from $name ----";

					my $rep = Dispatch($hash, $params[1] . " paired " . $params[1] . ":OK" );
					
					if(!(defined $rep))
					{					
						my $devname = "SHRDZM_" . $params[1];
						my $define= "$devname SHRDZMDevice $params[1]";

						Log3 $hash->{NAME}, 5, "create new device '$define'";
						my $cmdret= CommandDefine(undef,$define);	
						$cmdret= CommandAttr(undef,"$devname room SHRDZM");			
						$cmdret= CommandAttr(undef,"$devname IODev $hash->{NAME}");							
					}
				}
				elsif(substr($params[0], 5, 1) =~ "C")
				{
					Log3 $name, 5, "--- Config called ----";
				
					my @parameter = split(':', $params[2]);
					my $parameterSize = @parameter;

					if($parameterSize =~ 2)
					{
						Dispatch($hash, $params[1] . " config " . $params[2] );
					}
				}
				elsif(substr($params[0], 5, 1) =~ "D")
				{
					Log3 $name, 5, "--- Data called ----";
				
					my @parameter = split(':', $params[2]);
					my $parameterSize = @parameter;

					if($parameterSize =~ 2)
					{
						Dispatch($hash, $params[1] . " value " . $params[2] );
					}
				}
				elsif(substr($params[0], 5, 1) =~ "I") # INIT
				{
					Log3 $name, 5, "--- Init called ----";
				
					Dispatch($hash, $params[1] . " init " . "init:".$params[2] );				
				}
				elsif(substr($params[0], 5, 1) =~ "V") # Version
				{
					Log3 $name, 5, "--- Version called ----";
				
					Dispatch($hash, $params[1] . " version " . "version:".$params[2] );
				}				
				elsif(substr($params[0], 5, 1) =~ "X") # Supported sensors
				{
					Log3 $name, 5, "--- Supported sensors called ----";
				
					Dispatch($hash, $params[1] . " sensors " . "sensors:".$params[2] );
				}								
			}
		}
	}
	elsif($len =~ 2)
	{
		if(substr($params[0], 0, 1) =~ "~" && substr($params[0], 5, 1) =~ "G")
		{
			Log3 $name, 5, "--- Gateway called ----";
		
			my @parameter = split(':', $params[1]);
			my $parameterSize = @parameter;

			if($parameterSize =~ 2)
			{
				$hash->{GATEWAY} = $parameter[1];
			}
		}
	}
}

sub Read($)
{
  my ($hash) = @_;
  my $name = $hash->{NAME};
  
  my $data = DevIo_SimpleRead($hash);
  return if(!defined($data)); # connection lost
  
  my $buffer = $hash->{PARTIAL};
  
  
  # concat received data to $buffer
  $buffer .= $data;

  # as long as the buffer contains newlines (complete datagramm)
  while($buffer =~ m/\n/)
  {
    my $msg;
    
    # extract the complete message ($msg), everything else is assigned to $buffer
    ($msg, $buffer) = split("\n", $buffer, 2);
    
    # remove trailing whitespaces
    chomp $msg;
    	
    # parse the extracted message
    ParseMessage($hash, $msg);
  }

  # update $hash->{PARTIAL} with the current buffer content
  $hash->{PARTIAL} = $buffer; 
}

sub Undefine($$) 
{
    my ($hash, $name) = @_;

	if($hash->{Protocol} =~ "serial")
	{
	  DevIo_CloseDev($hash);
	  
	  return undef;	
	}
	else
	{
		foreach (@topics) {
			my $oldTopic = SHRDZM::DEVICE::GetTopicFor($hash, $_);
			client_unsubscribe_topic($hash, $oldTopic);

			Log3($hash->{NAME}, 5, "automatically unsubscribed from topic: " . $oldTopic);
		}

		return MQTT::Client_Undefine($hash);
	}
}

sub Set($$$@) 
{
    my ($hash, $name, $command, @values) = @_;

	if ($command eq '?' || $command =~ m/^(blink|intervals|(off-|on-)(for-timer|till)|toggle)/)
	{
		return SetExtensions($hash, 
			join(" ", map { "$_$SHRDZM_sets{$_}" } keys %SHRDZM_sets) . " " . join(" ", map {$hash->{sets}->{$_} eq "" ? $_ : "$_:".$hash->{sets}->{$_}} sort keys %{$hash->{sets}}), $name, $command, @values);
	}

	Log3($hash->{NAME}, 5, "cmnd = " . $command);

	if($hash->{Protocol} =~ "serial")
	{
		if($command =~ "pair")
		{
			DevIo_SimpleWrite($hash, "\$pair\n", 2);
		}
	}
	else
	{
		my $msgid;
		my $retain = $hash->{".retain"}->{'*'};
		my $qos = $hash->{".qos"}->{'*'};	
		my $value = join (" ", @values);
		
		if($command =~ "RCSend")
		{
			my $sendcommand = $value;
			my $topic = $hash->{FULL_TOPIC} . "RCSend";	
		
			$msgid = send_publish($hash->{IODev}, topic => $topic, message => $sendcommand, qos => $qos, retain => $retain);

			Log3($hash->{NAME}, 5, "sent (cmnd) '" . $value . "' to " . $topic);
		}
		else
		{	
			my @cList = %SHRDZM_sets;
			
			my $sendcommand = $command;
			my $topic = $hash->{FULL_TOPIC} . "set";	
			
			if(length($value) > 0)
			{
				$sendcommand .= " " . $value;
			}	
			
			$msgid = send_publish($hash->{IODev}, topic => $topic, message => $sendcommand, qos => $qos, retain => $retain);

			Log3($hash->{NAME}, 5, "sent (cmnd) '" . $value . "' to " . $topic);
		}
	}
	
	return;
}

sub onmessage($$$) # from mqtt
{
    my ($hash, $topic, $message) = @_;

	my @abc = split('/', $topic);
	my $len = @abc;
	my $name = $hash->{NAME};

    Log3($hash->{NAME}, 5, "received message '" . $message . "' for topic: " . $topic . " - " . $len . " items");

	if(substr($topic, 0, length($hash->{FULL_TOPIC})) =~ $hash->{FULL_TOPIC})
	{
		my $item = substr($topic, length($hash->{FULL_TOPIC}));		

		if($len =~ 3)
		{			
			if($item =~ "update")
			{
				if (eval {JSON::decode_json($message)}) 
				{
					my $json = JSON::decode_json($message);
					
					my $devices = $json->{devices};
					
					foreach my $key (keys %$devices) 
					{
						Log3($hash->{NAME}, 5, "device : ".$key);	

						my $rep = Dispatch($hash, $key . " paired " . $abc[1]."/". $key . ":OK" );

						if(!(defined $rep))
						{			
							my $devname = "SHRDZM_" . $key;
							my $define= "$devname SHRDZMDevice $key";

							Log3 $hash->{NAME}, 5, "create new device '$define'";
							my $cmdret= CommandDefine(undef,$define);	
							$cmdret= CommandAttr(undef,"$devname room SHRDZM");			
							$cmdret= CommandAttr(undef,"$devname IODev $hash->{NAME}");			
						}
						
						my $e = $devices->{$key};
						
						foreach my $entry (keys %$e) 
						{
							Log3($hash->{NAME}, 5, "entry : ".$entry." value : ".$e->{$entry} );	
							
							Dispatch($hash, $key . " config " . $entry.":".$e->{$entry} );
						}						
					}
				}
			}
			elsif($item =~ "state")
			{		
				readingsSingleUpdate($hash, $item, $message, 1);
			}
			elsif($item =~ "paired")
			{
				Log3 $hash->{NAME}, 5, "--- Pairing called from $name ----";
			
				my @parameter = split('/', $message);
				my $rep = Dispatch($hash, $parameter[1] . " paired " . $message . ":OK" );

				if(!(defined $rep))
				{			
					my $devname = "SHRDZM_" . substr($message, index($message, "/")+1);
					my $define= "$devname SHRDZMDevice $parameter[1]";

					Log3 $hash->{NAME}, 5, "create new device '$define'";
					my $cmdret= CommandDefine(undef,$define);	
					$cmdret= CommandAttr(undef,"$devname room SHRDZM");			
					$cmdret= CommandAttr(undef,"$devname IODev $hash->{NAME}");			
				}
			}
			elsif($item =~ "RCData")
			{
				readingsSingleUpdate($hash, $item, $message, 1);
			}
			elsif($item =~ "IP")
			{
				readingsSingleUpdate($hash, $item, $message, 1);
			}
			elsif($item =~ "version")
			{
				readingsSingleUpdate($hash, $item, $message, 1);
			}
			elsif($item =~ "gatewayaddress")
			{
				$hash->{GATEWAY} = $message;
			}			
		}
		elsif($len =~ 4)
		{
			if($abc[3] =~ "config")
			{
				Log3 $hash->{NAME}, 5, "sensor config $message";

				my @parameter = split(':', $message);
				my $parameterSize = @parameter;

				if($parameterSize =~ 2)
				{
					Dispatch($hash, $abc[2] . " config " . $message );
				}
			}
			elsif($abc[3] =~ "sensors")
			{
				Log3 $hash->{NAME}, 1, "sensors value $message";

				Dispatch($hash, $abc[2] . " sensors " . "sensors:".$message );
			}
			elsif($abc[3] =~ "sensor")
			{
				Log3 $hash->{NAME}, 5, "sensor value $message";

				my @parameter = split(':', $message);
				my $parameterSize = @parameter;

				if($parameterSize =~ 2)
				{
					Dispatch($hash, $abc[2] . " value " . $message );
				}
			}
			elsif($abc[3] =~ "init")
			{
				Log3 $hash->{NAME}, 5, "init $message";

				Dispatch($hash, $abc[2] . " init " . "init:".$message );
			}
			elsif($abc[3] =~ "version")
			{
				Dispatch($hash, $abc[2] . " version " . "version:".$message );
			}
			elsif($abc[3] =~ "param")
			{
				Log3 $hash->{NAME}, 5, "param $message";

				Dispatch($hash, $abc[2] . " param " . "param:".$message );
			}
			else
			{
				Log3 $hash->{NAME}, 1, "!!! $message !!!";
			}
		}
    } 
	else 
	{
        # Forward to "normal" logic
        MQTT::DEVICE::onmessage($hash, $topic, $message);
    }
}

sub GetTopicFor($$) 
{
    my ($hash, $postfix) = @_;

    my $tempTopic = $hash->{FULL_TOPIC}.$postfix;

    return $tempTopic;
}

sub Attr($$$$) 
{
    my ($command, $name, $attribute, $value) = @_;
    my $hash = $defs{$name};

    my $result = MQTT::DEVICE::Attr($command, $name, $attribute, $value);

    if ($attribute eq "IODev") 
	{
        # Subscribe Readings
        foreach (@topics) 
		{
            my $newTopic = SHRDZM::DEVICE::GetTopicFor($hash, $_);
            my ($mqos, $mretain, $mtopic, $mvalue, $mcmd) = MQTT::parsePublishCmdStr($newTopic);
            MQTT::client_subscribe_topic($hash, $mtopic, $mqos, $mretain);

            Log3($hash->{NAME}, 5, "automatically subscribed to topic: " . $newTopic);
        }

        $hash->{READY} = 1;
    }

    return $result;
}

1;

=pod
=item helper
=item summary    Gateway which connects SHRDZMDevices via serial or MQTT
=begin html

<a name="SHRDZM"></a>
<h3>SHRDZM</h3>
<ul>

  Gateway which connects SHRDZMDevices via serial or MQTT.
  <br>
  <br>
	Note: MQTT mode requires a MQTT-device as IODev. <br/>
	This module is based on Net::MQTT which needs to be installed from CPAN first.
  <br><br>
	More detailed information about SHRDZM Sensor Integration Platform is available in the<br/>
	<a href="https://github.com/saghonfly/shrdzm/wiki/" target="_blank">SHRDZM Wiki</a>
  <br><br>
  <a name="SHRDZMdefine"></a>
  <b>Define</b>
  <ul>    
  
    <code>define &lt;name&gt; SHRDZM &lt;device&gt; &lt;devicename&gt; [&lt;serial-device&gt;]@9600</code><br/>
    <code>define &lt;name&gt; SHRDZM &lt;device&gt; &lt;mqtt-deviceid&gt;</code>
    <br><br>

	This module acts as gateway and generates automatically SHRDZMDevice instances when paired.
	<br/><br/>
    Example:
    <ul>
      <code>define serialSHRDZM SHRDZM B4E62D26F273 /dev/serial/by-path/platform-3f980000.usb-usb-0:1.5:1.0-port0@9600</code><br>
      <code>define mqttSHRDZM SHRDZM ECFABC0CE7A2</code><br>
    </ul>
  </ul>
  <br>

  <a name="Set"></a>
  <b>Set</b>
  <ul>
	<li>
		<p>
			<code>set &lt;RCSend&gt; &lt;value&gt</code><br>
			Sends a 433MHz code.<br/>
			Only available if Protocol is MQTT and if SHRDZM Integration Style is 
			<a href="https://github.com/saghonfly/shrdzm/raw/dev/hardware/IntegrationStyle_2a.png?raw=true" target="_blank">IIa</a>.<br/>
		</p>
	</li>
	<li>
		<p>
			<code>set &lt;pair&gt; &lt;value&gt</code><br>
			Sets the gateway to pairing mode.<br/>
		</p>
	</li>
	<li>
		<p>
			<code>set &lt;reset&gt;</code><br>
			Resets the gateway.<br/>
		</p>
	</li>
  </ul>
  <br>



</ul>

=end html


=cut