##############################################
# $Id: 69_SHRDZMDevice.pm 20665 2020-03-09 11:05:35Z Erich O. Pintar $
package main;

use strict;
use warnings;
use SetExtensions;

sub
SHRDZMDevice_Initialize($)
{
  my ($hash) = @_;

  $hash->{SetFn}     = "SHRDZMDevice_Set";
  $hash->{DefFn}     = "SHRDZMDevice_Define";
  $hash->{ParseFn}   = "SHRDZMDevice_Parse";
  no warnings 'qw';
  my @attrList = qw(
    disable
    disabledForIntervals
    readingList
    setExtensionsEvent:1,0
    setList
    useSetExtensions
	IODev
  );
  use warnings 'qw';
  $hash->{AttrList} = join(" ", @attrList)." $readingFnAttributes";
  
  $hash->{Match} = "\\S{12}\\s\\S+\\s{1}\\S+\:{1}\\S+";  
}

###################################
sub
SHRDZMDevice_Set($@)
{
	my ( $hash, $name, $cmd, @args ) = @_;
	my $setList = AttrVal($name, "setList", " ");
	$setList =~ s/\n/ /g;
	
	return "\"set $name\" needs at least one argument" unless(defined($cmd));

	if ($cmd eq '?' || $cmd =~ m/^(blink|intervals|(off-|on-)(for-timer|till)|toggle)/)
	{
		return "Unknown argument $cmd, choose one of $setList";
	}
	
	my $ret = IOWrite($hash, $hash->{DEF} . " " . $cmd . " " . join(" ", @args));
		
	return undef;
}

sub SHRDZMDevice_Parse ($$)
{
	my ( $io_hash, $message) = @_;
		
	# Die Stellen 1-12 enthalten die eindeutige Identifikation des Geräts
	my $address = substr($message, 0, 12); 
	my @items = split(" ", $message);

	if(my $hash = $modules{SHRDZMDevice}{defptr}{$address})
	{
		my @parameter = split(":", $items[2]);
		
		if($items[1] =~ "value")
		{	
			my $rv = readingsSingleUpdate($hash, $parameter[0], $parameter[1], 1);
		
			return $hash->{NAME};
		}
#		elsif($items[1] =~ "config")
#		{
#			my $sl = AttrVal($hash->{NAME}, "setList", undef);		
			
#			if($sl == undef)
#			{
#				fhem( "attr " . $devname ." setList ".join(" ", @existing));

#			}
#		}		
	}
	else
	{
		# Keine Gerätedefinition verfügbar
		# Daher Vorschlag define-Befehl: <NAME> <MODULNAME> <ADDRESSE>
#		return "UNDEFINED SHRDZM_".$address." SHRDZMDevice $address";
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
