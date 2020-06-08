##############################################
# $Id: 69_SHRDZMDevice.pm 20665 2020-03-09 11:05:35Z Erich O. Pintar $
package main;

use strict;
use warnings;
use SetExtensions;
use GPUtils qw(:all);

my $offlinecounter = 0;
	
BEGIN {
    GP_Import(qw(
		CommandDefine
        CommandDeleteReading
        CommandAttr
        readingsSingleUpdate
        readingsBulkUpdate
        readingsBeginUpdate
        readingsEndUpdate
		readingsDelete
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

sub
SHRDZMDevice_Initialize($)
{
  my ($hash) = @_;

  $hash->{SetFn}     = "SHRDZMDevice_Set";
  $hash->{GetFn}     = "SHRDZMDevice_Get";
  $hash->{DefFn}     = "SHRDZMDevice_Define";
  $hash->{ParseFn}   = "SHRDZMDevice_Parse";
  $hash->{UndefFn}   = "SHRDZMDevice_Undef";
  $hash->{FingerprintFn}   = "SHRDZMDevice_Fingerprint";
  
    
  no warnings 'qw';
  my @attrList = qw(
    disable
    disabledForIntervals
    readingList
    setExtensionsEvent:1,0
    setList
    useSetExtensions
	IODev
	upgradePath
	offlinesensitivity:1,2,3,4,5
  );
  use warnings 'qw';
  $hash->{AttrList} = join(" ", @attrList)." $readingFnAttributes";
  
  $hash->{Match} = "\\S{12}\\s\\S+\\s{1}\\S+\:{1}\\S+";  
}

###################################
sub
SHRDZMDevice_Get($$@)
{
	my ( $hash, $name, $opt, @args ) = @_;

	return "\"get $name\" needs at least one argument" unless(defined($opt));

	if($opt eq "configuration") 
	{
		Log3($hash->{NAME}, 1, $hash->{NAME} . " Get configuration called ");
		
		my $ret = IOWrite($hash, $hash->{DEF} . " configuration");	

		return "Device ".
		$hash->{NAME}.
		" will be refreshed next time when it is up. Please be patient...";
	}
	else
	{
		return "Unknown argument $opt, choose one of configuration:noArg";
	}
}

sub
SHRDZMDevice_Set($@)
{
	my ( $hash, $name, $cmd, @args ) = @_;
	my $setList = AttrVal($name, "setList", " ");
	$setList =~ s/\n/ /g;	
		
	if ($cmd eq '?' || $cmd =~ m/^(blink|intervals|(off-|on-)(for-timer|till)|toggle)/)
	{	
		return "Unknown argument $cmd, choose one of "."upgrade:noArg ".ReadingsVal($name, ".SETS", ""). " devicetype:".ReadingsVal($name, ".SENSORS", "");
	}

	if($cmd eq "upgrade")
	{
		if($hash->{IODev}{Protocol} eq 'serial')
		{
			return "Device ".
			$hash->{NAME}.
			" is not possible to upgrade via OTA because it is connected with a serial gateway.";		
		}
		
		Log3($hash->{NAME}, 5, $hash->{NAME} . " Upgrade choosen " . AttrVal($hash->{NAME}, "upgradePath", "http\://shrdzm.pintarweb.net/upgrade.php"));
		
		my $ret = IOWrite($hash, $hash->{DEF} . " " . $cmd . " " . AttrVal($hash->{NAME}, "upgradePath", "http\://shrdzm.pintarweb.net/upgrade.php"));		
		
		return "Device ".
		$hash->{NAME}.
		" will be upgraded next time when it is up. Please be patient...";		
	}
	else
	{
		Log3($hash->{NAME}, 5, $hash->{NAME} . " !!!! ".join(" ", @args));
		my $ret = IOWrite($hash, $hash->{DEF} . " " . $cmd . " " . join(" ", @args));
		
		return $cmd.		
		" will be changed next time when Device ".$hash->{NAME}." is up. Please be patient...";		
	}
		
	return undef;
}
sub SHRDZMDevice_Undef($$)
{
	my ( $hash, $name) = @_;   

	delete($modules{SHRDZMDevice}{defptr}{$hash->{DEF}});
	
	RemoveInternalTimer($hash, "SHRDZMDevice_GetUpdate");    
	return undef;  
}

sub SHRDZMDevice_GetUpdate ($$)
{
	my ($hash) = @_;
	my $name = $hash->{NAME};

	$offlinecounter ++;

	Log3 $hash->{NAME}, 1, "Offlinetimer : $offlinecounter ";

	if($offlinecounter >= AttrVal($hash->{NAME}, "offlinesensitivity", "1"))
	{
		readingsSingleUpdate($hash, "online", "0", 1);
		$offlinecounter = 0;
		Log3($hash->{NAME}, 5, $hash->{NAME} . " went OFFLINE");
	}
	else
	{
		my $sl = ReadingsVal($hash->{NAME}, "interval", 0);
		my $t1 = gettimeofday()+($sl*2);
	
		InternalTimer($t1, "SHRDZMDevice_GetUpdate", $hash);							
	}
}

sub SHRDZMDevice_Fingerprint($$)
{
  my ( $io_name, $msg ) = @_;

  substr( $msg, 0, 12, "------------" ); # entferne Empfangsadresse

  return ( $io_name, $msg );
}

sub SHRDZMDevice_Parse ($$)
{
	my ( $io_hash, $message) = @_;
		
	# Die Stellen 0-12 enthalten die eindeutige Identifikation des Geräts
	my $address = substr($message, 0, 12); 
	my @items = split(" ", $message);


	if(my $hash = $modules{SHRDZMDevice}{defptr}{$address})
	{
		Log3($hash->{NAME}, 5, $hash->{NAME} . "parse message : $message");


		my @parameter = split(":", $items[2]);
		
		if($items[1] =~ "value")
		{	
			RemoveInternalTimer($hash, "SHRDZMDevice_GetUpdate");

			readingsBeginUpdate($hash);
			readingsBulkUpdate($hash, $parameter[0], $parameter[1], 1);			
			my $oldOnlineState = ReadingsVal($hash->{NAME}, "online", 0);
			if($oldOnlineState =~ 0)
			{
				readingsBulkUpdate($hash, "online", "1", 1);
				$offlinecounter = 0;
			}
			readingsEndUpdate($hash, 1);

			
			my $sl = ReadingsVal($hash->{NAME}, "interval", 0);
			my $t1 = gettimeofday()+($sl*2);
			
			InternalTimer($t1, "SHRDZMDevice_GetUpdate", $hash);							
		
			return $hash->{NAME};
		}
		elsif($items[1] =~ "paired")
		{
			Log3 $hash->{NAME}, 5, "RePairing of ".$hash->{NAME}." to ".$io_hash->{NAME}." with  = ".$message;

			$hash->{IODev} = $io_hash->{NAME};
			CommandAttr(undef,"$hash->{NAME} IODev $io_hash->{NAME}");
			
			return $hash->{NAME};		
		}
		elsif($items[1] =~ "version")
		{
			$hash->{VERSION} = $parameter[1];

			return $hash->{NAME};		
		}
		elsif($items[1] =~ "init")
		{
			# delete all readings
			my @cList = keys %{$hash->{READINGS}};
			foreach my $oldReading (@cList)
			{
				if(!($oldReading =~ ".SENSORS"))
				{
					Log3 $hash->{NAME}, 1, "will delete $oldReading";
					
					readingsDelete($hash, $oldReading);
				}
			}

			CommandAttr(undef,"$hash->{NAME} upgradePath http\://shrdzm.pintarweb.net/upgrade.php");	

			return $hash->{NAME};		
		}
		elsif($items[1] =~ "sensors")
		{			
			Log3($hash->{NAME}, 5, $hash->{NAME} . "!!!sensors updated : $parameter[1]");
		
			readingsSingleUpdate($hash, ".SENSORS", $parameter[1], 0);
			
			return $hash->{NAME};		
		}
		elsif($items[1] =~ "config")
		{
			if(!($parameter[0] =~ "devicetype"))
			{
				my $sl = ReadingsVal($hash->{NAME}, ".SETS", "");
				
				my @existing = split(' ', $sl);
							
				if ( !($parameter[0] ~~ @existing ))
				{
					push(@existing, $parameter[0]);

					readingsSingleUpdate($hash, ".SETS", join(" ", @existing), 0);				
				}
			}
			
			my $rv = readingsSingleUpdate($hash, $parameter[0], $parameter[1], 1);
			$hash->{STATE} = "OK";

			if($parameter[0] =~ "interval")
			{
				RemoveInternalTimer($hash, "SHRDZMDevice_GetUpdate");
				
				my $t1 = gettimeofday()+($parameter[1]*2);
			
				InternalTimer($t1, "SHRDZMDevice_GetUpdate", $hash);							
			}

			return $hash->{NAME};
		}	
		elsif($items[1] =~ "version")
		{
			$hash->{VERSION} = $message;

			return $hash->{NAME};		
		}		
	}
	else
	{
		# Keine Gerätedefinition verfügbar
		# Daher Vorschlag define-Befehl: <NAME> <MODULNAME> <ADDRESSE>
#		return "UNDEFINED SHRDZM_".$address." SHRDZMDevice $address";

		Log3($hash->{NAME}, 5, $hash->{NAME} . "!!!parse message : $message");

		return undef;
	}
}

sub
SHRDZMDevice_Define($$)
{
	my ( $hash, $def) = @_;
	my @a = split("[ \t][ \t]*", $def);
	my $name = $a[0];

	# erstes Argument ist die eindeutige Geräteadresse
	my $address = $a[2];

	# Adresse rückwärts dem Hash zuordnen (für ParseFn)
	$modules{SHRDZMDevice}{defptr}{$address} = $hash;  

	return "Invalid number of arguments: define <name> SHRDZMDevice identifier" if (int(@a) < 2);

	AssignIoPort($hash);

	return undef;
}

1;

=pod
=item helper
=item summary    Single SHRDZM sensor device
=begin html

<a name="SHRDZMDevice"></a>
<h3>SHRDZMDevice</h3>
<ul>

  Single SHRDZM device which is connected via EspNow protocol to a SHRDZM Gateway.
  <br>
  <br>
	More detailed information about SHRDZM Sensor Integration Platform is available in the<br/>
	<a href="https://github.com/saghonfly/shrdzm/wiki/" target="_blank">SHRDZM Wiki</a>
  <br><br>
  <a name="SHRDZMDevice_Define"></a>
  <b>Define</b>
  <ul>    
  
    <code>define &lt;name&gt; SHRDZMDevice &lt;unique-deviceid&gt;</code><br/>
    <br><br>

	This module represents a single SHRDZM sensor device.<br>
	SHRDZMDevice will be crated automatically by a SHRDZM Gateway during pairing.
	<br/><br/>
  </ul>
  <br>

  <a name="SHRDZMDevice_Set"></a>
  <b>Set</b>
  <ul>
	<li>
		<p>
			<code>set &lt;devicetype&gt; &lt;value&gt</code><br>
			Sets the type of the sensor.<br/>
		</p>
	</li>
	<li>
		<p>
			<code>set &lt;upgrade&gt;</code><br>
			Will update the physical SHRDZMDevice sensor module with the newest firmware defined in the upgradePath attribute.<br/>
			This is only working if the device is connected via MQTT Gateway.
		</p>
	</li>
  </ul>
  <br>

  <a name="SHRDZMDevice_Get"></a>
  <b>Get</b>
  <ul>
	<li>
		<p>
			<code>get &lt;configuration&gt;</code><br>
			Configuration will be refreshed at next time when the sensor is up.<br/>
		</p>
	</li>
  </ul>
  <br>

  <a name="SHRDZMDevice_Attr"></a>
  <b>Attributes</b>
  <ul>
	<li>
      <p>
		<code>attr &lt;name&gt; upgradePath &lt;http-address&gt;</code><br/>
        specify the http-path which provides the newest sensor firmware for over-the-air upgrade.<br/>
		This is only working if the device is connected via MQTT Gateway. 
	  </p>	
	</li>
  </ul>
  <br>

</ul>

=end html


=cut