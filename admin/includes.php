<?php
/****
Copyright 2010 Monotype Imaging Inc.  
This program is distributed under the terms of the GNU General Public License
****/

/*start defining global variables*/
define("TBLCONFIG","{wfs_configure}");
/*wfsapi*/
define("REQESTURI","http://api.fonts.com");
define("FASTFONTURI","http://fast.fonts.com");
define("FONTFCURI",FASTFONTURI."/d/");
define("FFJSAPIURI",FASTFONTURI."/jsapi/");
define("FFCSSHDLRURI",FASTFONTURI."/cssapi/");
define("SIGNUPURI","https://webfonts.fonts.com/en-US/Subscription/SelectSubscription");
define("GETKEYURI","https://webfonts.fonts.com/en-US/Account/AccountInformation");
/*PAGINATIN VALUE*/
define("DOMAIN_LIMIT",10);
define("SELECTOR_LIMIT",10);
define("FONT_LIMIT",10);
define("PROJECT_LIMIT",10);
/*end defining global variables*/


/*
* getting user details
*/
function getUnPass(){
	$webfonts_userid = variable_get('webfonts_userid','');
	$webfonts_public_key = variable_get('webfonts_public_key','');;
	$webfonts_private_key = variable_get('webfonts_private_key','');;
	$webfonts_usertype = variable_get('webfonts_usertype','');
	return array($webfonts_userid,$webfonts_public_key,$webfonts_private_key,$webfonts_usertype);
}

/*begin for selectors*/
function getAllActiveSelectors(){
	global $base_path;
	$sqlcurTheme="SELECT * FROM {system} WHERE `type`='theme' AND `status`='1' LIMIT 1";
	$results = db_query($sqlcurTheme);
	$fields = db_fetch_array($results);
	$wfs_info=unserialize($fields['info']);
	$wfs_style=$wfs_info['stylesheets']['all'];
	
	$wfs_selector=array();
	if(is_array($wfs_style)){
		foreach($wfs_style as $key=>$value){
			$new_arr=getFileContent($value);
			$wfs_selector=array_merge(array_unique($new_arr),$wfs_selector);
		}
	}
	return $wfs_selector;
}
/*
* getting the file content from theme files
*/
function getFileContent($filename){
	$handle=fopen($filename,'r');
	$fileCont = fread($handle, filesize($filename));
	fclose($handle);
	$fileCont = preg_replace('/\/\\*.*?\\*\//s', '', $fileCont);
	$fileCont = preg_replace('/{.*?}/s', '::', $fileCont);
	$fileCont = preg_split('/::/',$fileCont,0,PREG_SPLIT_NO_EMPTY);
	foreach($fileCont as $value){
		$trmval=trim(preg_replace('/\\r\\n/', '', $value));
		if($trmval!=''){
			if($pos = strpos($trmval,",")){
				$further_exploded=explode(",",$trmval);//for exploding all the comma separated values
				foreach($further_exploded as $fexp){
					$newarr[]=trim($fexp);
				}
			}else{
				$newarr[]=$trmval;
			}			
		}
	}
	return $newarr;
}

 /*
 * Checking the day for which the current project should be fetched
 */ 
function checkday($projectDay){
	$today=date("w");
	//checkforminus sign
	$checkme="-";
	$returnValue=false;
	
	$pos = strpos($projectDay,$checkme);
	if($pos === false){
		$pos1=strpos($projectDay,",");
		if($pos1===false){
			if($today==$projectDay){
				$returnValue=true;
			}
		}else{
			$days=explode(",",$projectDay);
			foreach($days as $day){
				//checkif the "-" exists further...
				if(strpos($day,$checkme)){
					$day12=explode($checkme,$day);
					for($j=$day12[0];$j<=$day12[1];$j++){
						if($j>6){
							break;
						}
						if($j==$today){
							$returnValue=true;
						}
					}
				}else if($day==$today){
					$returnValue=true;
				}
			}
		}
	}else{
		$days=explode($checkme,$projectDay);
		for($i=$days[0];$i<=$days[1];$i++){
			if($i>6){
				break;
			}
			if($i==$today){
				$returnValue=true;
			}
		}
	}
	return $returnValue;
}

/**
Browser checking function
*/
function browserName(){
if ( strpos($_SERVER['HTTP_USER_AGENT'], 'Gecko') )
	{
		if ( strpos($_SERVER['HTTP_USER_AGENT'], 'Netscape') )
   			{
     			$browser = 'Netscape (Gecko/Netscape)';
   			}
		else if ( strpos($_SERVER['HTTP_USER_AGENT'], 'Firefox') )
   			{
     		$browser = 'Mozilla Firefox (Gecko/Firefox)';
   			}
   		else
   			{
     		$browser = 'Mozilla (Gecko/Mozilla)';
   			}
		}
else if ( strpos($_SERVER['HTTP_USER_AGENT'], 'MSIE') )
		{
   		if ( strpos($_SERVER['HTTP_USER_AGENT'], 'Opera') )
   			{
     			$browser = 'Opera (MSIE/Opera/Compatible)';
   			}
   		else
  		 	{
     		$browser = 'Internet Explorer (MSIE/Compatible)';
   			}
		}
	else
	{
   		$browser = 'Others browsers';
	}
return $browser;
}
/* Page Visibility function*/
function pageMatch($visibility_pages,$visibility,$path){
	if ($visibility_pages) {
    	if ($visibility <  2) {
          	// Compare with the internal and path alias (if any).
    	    $page_match = drupal_match_path($path, $visibility_pages);
      		if ($path != $_GET['q']) {
        	$page_match = $page_match || drupal_match_path($_GET['q'], $visibility_pages);
      			}
      // When $visibility has a value of 0, the block is displayed on
      // all pages except those listed in $visibility_pages. When set to 1, it
      // is displayed only on those pages listed in $visibility_pages.
	
     	$page_match = !($visibility xor $page_match);
   
		}
    	else {
      		$page_match = drupal_eval($visibility_pages);
    		}
  	}
  	else {
    	$page_match = TRUE;
  	}
return $page_match; 
}
?>