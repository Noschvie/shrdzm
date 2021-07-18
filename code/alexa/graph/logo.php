<?php
header("Content-type: image/png");
$imgWidth = '200';
$imgHeight = '100';
$theImage = imagecreate($imgWidth, $imgHeight);
$colorGrey = imagecolorallocate($theImage, 215, 215, 215);
$colorBlack = imagecolorallocate($theImage, 0, 0, 0);
$colorBlue = imagecolorallocate($theImage, 0, 50, 255);

imagecolortransparent($theImage, $colorGrey);

$wert[0] = '50';
$wert[1] = '100';
$wert[2] = '24';
$wert[3] = '30';
$wert[4] = '90';
$wert[5] = '102';
$wert[6] = '56';
$wert[7] = '23';


$countBalken = count($wert);
for($i=0;$i<$countBalken;$i++)
{
	if($maxHeight < $wert[$i])
	{
		$maxHeight = $wert[$i];
	}
}
$dynWidth = round(($imgWidth-2) / ($countBalken-1), 0);
@$dynHeight = ($imgHeight-1) / $maxHeight;
for ($i=0; $i<$countBalken; $i++)
{
	$aktNewSize = @round($dynHeight * $wert[$i]);
	$aktNewSize2 = @round($dynHeight * $wert[$i+1]);
	if($i+1 < $countBalken)
	{
		imageline($theImage, $i*$dynWidth, ($imgHeight-$aktNewSize),
		($i+1)*$dynWidth, ($imgHeight-$aktNewSize2), $colorBlue);
	}
}

imageline($theImage, 0, 0, $imgWidth, 0, $colorBlack); #oben
imageline($theImage, $imgWidth-1, 0, $imgWidth-1, $imgHeight-1, $colorBlack); #rechts
imageline($theImage, 0, $imgHeight-1, $imgWidth, $imgHeight-1, $colorBlack); #unten
imageline($theImage, 0, 0, 0, $imgHeight, $colorBlack); #links

$pxRaster = '20'; #Größe des Rasters
$pxWidth = round($imgWidth / $pxRaster, 0);
for($i=1; $i<$pxWidth; $i++)
{
	imageline($theImage, $i*$pxRaster, 0, $i*$pxRaster, $imgHeight, $colorBlack); #vertikal
	imageline($theImage, 0, $i*$pxRaster, $imgWidth, $i*$pxRaster, $colorBlack); #horizontal
}

imagepng($theImage);
imagedestroy($theImage);
?>