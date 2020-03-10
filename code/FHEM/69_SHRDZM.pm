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

my %SHRDZM_gets = (
	"version"	=> ""
);

my %SHRDZM_sets = (
    "send" => "",
    "reset" => ":noArg",
    "pair" => ":60,120,180"
);

my @topics = (
    "state",
    "paired",
	"+/config",
	"+/sensor",
);


sub SHRDZM_Initialize($) {
	my $hash = shift @_;
	
	require "$main::attr{global}{modpath}/FHEM/00_MQTT.pm";	

    $hash->{DefFn}      = "SHRDZM::DEVICE::Define";
    $hash->{WriteFn}    = "SHRDZM::DEVICE::Write";
	$hash->{UndefFn} 	= "SHRDZM::DEVICE::Undefine";
    $hash->{SetFn}      = "SHRDZM::DEVICE::Set";
	$hash->{AttrFn} 	= "SHRDZM::DEVICE::Attr";
    $hash->{AttrList} 	= "IODev qos retain publishSet publishSet_.* subscribeReading_.* autoSubscribeReadings " . $main::readingFnAttributes;
    $hash->{OnMessageFn} = "SHRDZM::DEVICE::onmessage";

	$hash->{Clients} = "SHRDZMDevice";
  
	$hash->{MatchList} = { "1:SHRDZMDevice" => "\\S{12}\\s\\S+\\s{1}\\S+\:{1}\\S+" };  
  
		# \\S{12}\\s{1}\\S+\:{1}\\S+
  
    main::LoadModule("MQTT");
    main::LoadModule("MQTT_DEVICE");
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

use Net::MQTT::Constants;

BEGIN {
    MQTT->import(qw(:all));

    GP_Import(qw(
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

    return "Invalid number of arguments: define <name> SHRDZM <gatewayID>" if (int(@args) < 1);

		
	my ($name, $type, $topic) = @args;

    if (defined($topic)) 
	{

        $hash->{TOPIC} = $topic;
        $hash->{MODULE_VERSION} = "0.5";
        $hash->{READY} = 0;

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
};

sub Write ($$)
{
	my ( $hash, @arguments) = @_;

    Log3($hash->{NAME}, 5, "Bin im Write vom SHRDZM. Parameter = ".join(" ", @arguments));
	
	my $return = " funktioniert";
	
	my $topic = $hash->{FULL_TOPIC} . "config/set";	
	my $retain = $hash->{".retain"}->{'*'};
	my $qos = $hash->{".qos"}->{'*'};	

	my $msgid = send_publish($hash->{IODev}, topic => $topic, message => join(" ", @arguments), qos => $qos, retain => $retain);
	
#	return $return;
}

sub Undefine($$) {
    my ($hash, $name) = @_;

    foreach (@topics) {
        my $oldTopic = SHRDZM::DEVICE::GetTopicFor($hash, $_);
        client_unsubscribe_topic($hash, $oldTopic);

        Log3($hash->{NAME}, 5, "automatically unsubscribed from topic: " . $oldTopic);
    }

    return MQTT::Client_Undefine($hash);
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

	my @cList = %SHRDZM_sets;
	
	my $msgid;
	my $topic = $hash->{FULL_TOPIC} . "set";	
	my $retain = $hash->{".retain"}->{'*'};
	my $qos = $hash->{".qos"}->{'*'};	
	my $value = join (" ", @values);
	my $sendcommand = $command;
	
	if(length($value) > 0)
	{
		$sendcommand .= " " . $value;
	}	
	
	$msgid = send_publish($hash->{IODev}, topic => $topic, message => $sendcommand, qos => $qos, retain => $retain);

	Log3($hash->{NAME}, 5, "sent (cmnd) '" . $value . "' to " . $topic);
		
	return;
}

sub onmessage($$$) 
{
    my ($hash, $topic, $message) = @_;

	my @abc = split('/', $topic);
	my $len = @abc;

    Log3($hash->{NAME}, 5, "received message '" . $message . "' for topic: " . $topic . " - " . $len . " items");

	if(substr($topic, 0, length($hash->{FULL_TOPIC})) =~ $hash->{FULL_TOPIC})
	{
		my $item = substr($topic, length($hash->{FULL_TOPIC}));		

		if($len =~ 3)
		{
			if($item =~ "state")
			{		
				readingsSingleUpdate($hash, $item, $message, 1);
			}
			elsif($item =~ "paired")
			{
				my @parameter = split('/', $message);
			
				my $devname = "SHRDZM_" . substr($message, index($message, "/")+1);
				my $define= "$devname SHRDZMDevice $parameter[1]";

				Log3 $hash->{NAME}, 5, "create new device '$define'";
				my $cmdret= CommandDefine(undef,$define);	
				$cmdret= CommandAttr(undef,"$devname room SHRDZM");			
				$cmdret= CommandAttr(undef,"$devname IODev $hash->{NAME}");			
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
=item [device]
=item summary TASMOTA_DEVICE acts as a fhem-device that is mapped to mqtt-topics of the custom tasmota firmware
=begin html
<a name="TASMOTA_DEVICE"></a>
<h3>TASMOTA_DEVICE</h3>
<ul>
  <p>acts as a fhem-device that is mapped to <a href="http://mqtt.org/">mqtt</a>-topics.</p>
  <p>requires a <a href="#MQTT">MQTT</a>-device as IODev<br/>
     Note: this module is based on <a href="https://metacpan.org/pod/distribution/Net-MQTT/lib/Net/MQTT.pod">Net::MQTT</a> which needs to be installed from CPAN first.</p>
  <a name="TASMOTA_DEVICEdefine"></a>
  <p><b>Define</b></p>
  <ul>
    <p><code>define &lt;name&gt; TASMOTA_DEVICE &lt;topic&gt; [&lt;fullTopic&gt;]</code><br/>
       Specifies the MQTT Tasmota device.</p>
  </ul>
  <a name="TASMOTA_DEVICEset"></a>
  <p><b>Set</b></p>
  <ul>
    <li>
      <p><code>set &lt;name&gt; &lt;command&gt;</code><br/>
         sets reading 'state' and publishes the command to topic configured via attr publishSet</p>
    </li>
    <li>
      <p><code>set &lt;name&gt; &lt;reading&gt; &lt;value&gt;</code><br/>
         sets reading &lt;reading&gt; and publishes the command to topic configured via attr publishSet_&lt;reading&gt;</p>
    </li>
  </ul>
  <a name="TASMOTA_DEVICEattr"></a>
  <p><b>Attributes</b></p>
  <ul>
    <li>
      <p><code>attr &lt;name&gt; publishSet [[&lt;reading&gt;:]&lt;commands_or_options&gt;] &lt;topic&gt;</code><br/>
         configures set commands and UI-options e.g. 'slider' that may be used to both set given reading ('state' if not defined) and publish to configured topic</p>
      <p>example:<br/>
      <code>attr mqttest publishSet on off switch:on,off level:slider,0,1,100 /topic/123</code>
      </p>
    </li>
    <li>
      <p><code>attr &lt;name&gt; publishSet_&lt;reading&gt; [&lt;values&gt;]* &lt;topic&gt;</code><br/>
         configures reading that may be used to both set 'reading' (to optionally configured values) and publish to configured topic</p>
    </li>
    <li>
      <p><code>attr &lt;name&gt; autoSubscribeReadings &lt;topic&gt;</code><br/>
         specify a mqtt-topic pattern with wildcard (e.c. 'myhouse/kitchen/+') and TASMOTA_DEVICE automagically creates readings based on the wildcard-match<br/>
         e.g a message received with topic 'myhouse/kitchen/temperature' would create and update a reading 'temperature'</p>
    </li>
    <li>
      <p><code>attr &lt;name&gt; subscribeReading_&lt;reading&gt; [{Perl-expression}] [qos:?] [retain:?] &lt;topic&gt;</code><br/>
         mapps a reading to a specific topic. The reading is updated whenever a message to the configured topic arrives.<br/>
         QOS and ratain can be optionally defined for this topic. <br/>
         Furthermore, a Perl statement can be provided which is executed when the message is received. The following variables are available for the expression: $hash, $name, $topic, $message. Return value decides whether reading is set (true (e.g., 1) or undef) or discarded (false (e.g., 0)).
         </p>
      <p>Example:<br/>
         <code>attr mqttest subscribeReading_cmd {fhem("set something off")} /topic/cmd</code>
       </p>
    </li>
    <li>
      <p><code>attr &lt;name&gt; retain &lt;flags&gt; ...</code><br/>
         Specifies the retain flag for all or specific readings. Possible values are 0, 1</p>
      <p>Examples:<br/>
         <code>attr mqttest retain 0</code><br/>
         defines retain 0 for all readings/topics (due to downward compatibility)<br>
         <code> retain *:0 1 test:1</code><br/>
         defines retain 0 for all readings/topics except the reading 'test'. Retain for 'test' is 1<br>
       </p>
    </li>
    <li>
      <p><code>attr &lt;name&gt; qos &lt;flags&gt; ...</code><br/>
         Specifies the QOS flag for all or specific readings. Possible values are 0, 1 or 2. Constants may be also used: at-most-once = 0, at-least-once = 1, exactly-once = 2</p>
      <p>Examples:<br/>
         <code>attr mqttest qos 0</code><br/>
         defines QOS 0 for all readings/topics (due to downward compatibility)<br>
         <code> retain *:0 1 test:1</code><br/>
         defines QOS 0 for all readings/topics except the reading 'test'. Retain for 'test' is 1<br>
       </p>
    </li>
  </ul>
</ul>
=end html
=cut