##############################################
# $Id: 69_SHRDZMDevice.pm 20665 2020-03-09 11:05:35Z Erich O. Pintar $
package main;

use strict;
use warnings;
use SetExtensions;
use GPUtils qw(:all);
#use Time::HiRes qw( usleep ualarm gettimeofday tv_interval nanosleep
#					clock_gettime clock_getres clock_nanosleep clock
 #                   stat lstat utime);
					  
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
		Log3($hash->{NAME}, 1, $hash->{NAME} . " Upgrade choosen " . AttrVal($hash->{NAME}, "upgradePath", "http\://shrdzm.pintarweb.net/upgrade.php"));
		
		my $ret = IOWrite($hash, $hash->{DEF} . " " . $cmd . " " . AttrVal($hash->{NAME}, "upgradePath", "http\://shrdzm.pintarweb.net/upgrade.php"));		
	}
	else
	{
		Log3($hash->{NAME}, 1, $hash->{NAME} . " !!!! ".join(" ", @args));
		my $ret = IOWrite($hash, $hash->{DEF} . " " . $cmd . " " . join(" ", @args));
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

	readingsSingleUpdate($hash, "online", "0", 1);	

	Log3($hash->{NAME}, 1, $hash->{NAME} . " went OFFLINE");
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
		Log3($hash->{NAME}, 1, $hash->{NAME} . "parse message : $message");


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
			Log3($hash->{NAME}, 1, $hash->{NAME} . "!!!sensors updated : $parameter[1]");
		
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
			
				Log3($hash->{NAME}, 5, "jetzt = ".localtime(gettimeofday()).", timer auf ".localtime($t1));
				
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

		Log3($hash->{NAME}, 1, $hash->{NAME} . "!!!parse message : $message");

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

#	readingsSingleUpdate($hash, "upgrade", "http\://shrdzm.pintarweb.net/upgrade.php", 1);

	AssignIoPort($hash);

	return undef;
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
