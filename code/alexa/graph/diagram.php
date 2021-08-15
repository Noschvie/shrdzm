<?php
require '../../../phplot/phplot.php';
require __DIR__.'/../classes/Database.php';
require __DIR__.'/../logging.php';

$size = explode("x", $_GET['size']);
$token = $_GET['token'];
$reading = $_GET['reading'];
$device = $_GET['device'];
$unit = $_GET['unit'];
$timeframe = $_GET['timeframe'];
//$desc = $_GET['desc'];

$plotData = array();

function GetDeviceData($days, $device, $reading)
{
	global $plotData;
	$db_connection = new Database();
	$conn = $db_connection->dbConnection();

	try
	{
		$fetch_values_from_device = 'SELECT * FROM `data` WHERE `name`="'.$device.'" AND reading="'.$reading.'" and `timestamp` > CURRENT_TIMESTAMP - INTERVAL '.$days.' DAY order by timestamp desc';
		$query_values_from_device = $conn->prepare($fetch_values_from_device);
		$query_values_from_device->execute();
	
		while ($rowValue = $query_values_from_device->fetch(PDO::FETCH_ASSOC))
		{
			array_unshift($plotData, array($rowValue['timestamp'], strtotime($rowValue['timestamp']), $rowValue['value']));
		} 
	}
	catch(PDOException $e)
	{
		
	}	
}

//Define the object
$plot = new PHPlot($size[0], $size[1]);


if(!isset($timeframe))
	$timeframe = "1";

GetDeviceData($timeframe, $device, $reading);

$plot->SetDataType('data-data');
$plot->SetLineWidths(2);


$plot->SetXLabelType('time', '%H:%M');
//$plot->SetXLabelType('data');
$plot->SetBackgroundColor(black);
$plot->SetDataColors(blue);
$plot->SetTextColor(white);
$plot->SetDataValueLabelColor(white);
$plot->SetXTitleColor(white);
$plot->SetYTitleColor(blue);
$plot->SetGridColor(white);
$plot->SetLightGridColor(DimGrey);
$plot->SetTTFPath('/usr/share/fonts/truetype/dejavu/');
$plot->SetFontTTF('x_label', 'DejaVuSansMono.ttf', 12);
$plot->SetFontTTF('y_label', 'DejaVuSansMono.ttf', 12);
$plot->SetFontTTF('y_title', 'DejaVuSansMono.ttf', 20);
$plot->SetYTitle($unit);
$plot->SetBrowserCache(False);
$plot->SetDrawXGrid(True);

//$plot->SetXTickLabelPos('none');
$plot->SetXDataLabelPos('none');

$plot->SetDataValues($plotData);

//Draw it
$plot->DrawGraph();
?>