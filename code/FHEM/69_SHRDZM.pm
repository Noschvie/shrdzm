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
		Log3($hash->{NAME}, 0, "NEW serial device : " . $args[3]);
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

    Log3($hash->{NAME}, 5, "Bin im Write vom SHRDZM. Parameter = ".join(" ", @arguments));

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

    Log3($hash->{NAME}, 1, "received message '" . $message . "' for topic: " . $topic . " - " . $len . " items");

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
=item summary    dummy device
=item summary_DE dummy Ger&auml;t
=begin html

<a name="dummy"></a>
<h3>dummy</h3>
<ul>

  Define a dummy. A dummy can take via <a href="#set">set</a> any values.
  Used for programming.
  <br><br>

  <a name="dummydefine"></a>
  <b>Define</b>
  <ul>
    <code>define &lt;name&gt; dummy</code>
    <br><br>

    Example:
    <ul>
      <code>define myvar dummy</code><br>
      <code>set myvar 7</code><br>
    </ul>
  </ul>
  <br>

  <a name="dummyset"></a>
  <b>Set</b>
  <ul>
    <code>set &lt;name&gt; &lt;value&gt</code><br>
    Set any value.
  </ul>
  <br>

  <a name="dummyget"></a>
  <b>Get</b> <ul>N/A</ul><br>

  <a name="dummyattr"></a>
  <b>Attributes</b>
  <ul>
    <li><a href="#disable">disable</a></li>
    <li><a href="#disabledForIntervals">disabledForIntervals</a></li>
    <li><a name="readingList">readingList</a><br>
      Space separated list of readings, which will be set, if the first
      argument of the set command matches one of them.</li>

    <li><a name="setList">setList</a><br>
      Space separated list of commands, which will be returned upon "set name
      ?", so the FHEMWEB frontend can construct a dropdown and offer on/off
      switches. Example: attr dummyName setList on off </li>

    <li><a name="useSetExtensions">useSetExtensions</a><br>
      If set, and setList contains on and off, then the
      <a href="#setExtensions">set extensions</a> are available.<br>
      Side-effect: if set, only the specified parameters are accepted, even if
      setList contains no on and off.</li>

    <li><a name="setExtensionsEvent">setExtensionsEvent</a><br>
      If set, the event will contain the command implemented by SetExtensions
      (e.g. on-for-timer 10), else the executed command (e.g. on).</li>

    <li><a href="#readingFnAttributes">readingFnAttributes</a></li>
  </ul>
  <br>

</ul>

=end html

=begin html_DE

<a name="dummy"></a>
<h3>dummy</h3>
<ul>

  Definiert eine Pseudovariable, der mit <a href="#set">set</a> jeder beliebige
  Wert zugewiesen werden kann.  Sinnvoll zum Programmieren.
  <br><br>

  <a name="dummydefine"></a>
  <b>Define</b>
  <ul>
    <code>define &lt;name&gt; dummy</code>
    <br><br>

    Beispiel:
    <ul>
      <code>define myvar dummy</code><br>
      <code>set myvar 7</code><br>
    </ul>
  </ul>
  <br>

  <a name="dummyset"></a>
  <b>Set</b>
  <ul>
    <code>set &lt;name&gt; &lt;value&gt</code><br>
    Weist einen Wert zu.
  </ul>
  <br>

  <a name="dummyget"></a>
  <b>Get</b> <ul>N/A</ul><br>

  <a name="dummyattr"></a>
  <b>Attributes</b>
  <ul>
    <li><a href="#disable">disable</a></li>
    <li><a href="#disabledForIntervals">disabledForIntervals</a></li>
    <li><a name="readingList">readingList</a><br>
      Leerzeichen getrennte Liste mit Readings, die mit "set" gesetzt werden
      k&ouml;nnen.</li>

    <li><a name="setList">setList</a><br>
      Liste mit Werten durch Leerzeichen getrennt. Diese Liste wird mit "set
      name ?" ausgegeben.  Damit kann das FHEMWEB-Frontend Auswahl-Men&uuml;s
      oder Schalter erzeugen.<br> Beispiel: attr dummyName setList on off </li>

    <li><a name="useSetExtensions">useSetExtensions</a><br>
      Falls gesetzt, und setList enth&auml;lt on und off, dann sind die <a
      href="#setExtensions">set extensions</a> verf&uuml;gbar.<br>
      Seiteneffekt: falls gesetzt, werden nur die spezifizierten Parameter
      akzeptiert, auch dann, wenn setList kein on und off enth&auml;lt.</li>

    <li><a name="setExtensionsEvent">setExtensionsEvent</a><br>
      Falls gesetzt, enth&auml;lt das Event den im SetExtensions
      implementierten Befehl (z.Bsp. on-for-timer 10), sonst den
      Ausgef&uuml;rten (z.Bsp. on).</li>

    <li><a href="#readingFnAttributes">readingFnAttributes</a></li>
  </ul>
  <br>

</ul>

=end html_DE

=cut
