<?php
// $Id$

/****
Copyright 2010 Monotype Imaging Inc.  
This program is distributed under the terms of the GNU General Public License
****/

/**
 * @file
 * This file holds the functions for the web-fonts Admin Configuration.
 *
 */

/**
 * Menu callback; displays the webfont module config page.
 * Plesae do not make changes in the code without asking the 
 * the owner of this code
 */
 
include_once('includes.php');
include_once('wfs_pagination.php');
include_once('wfsapi.class.php');

/*
* this is the main function of login page...
*/
function webfonts_admin_settings(&$form_state, $param=0){
	drupal_add_js("$(document).ready(function() {showWFSlogo()});", 'inline');
	if($param == 1)
		{
			return GetLoginForm();
			die();	
		}
		if(isset($_POST['op']) && $_POST['op']=='Reset Configuration'){
			drupal_goto($_POST['return_url']);
			die();
		}
		if(isset($_POST['webfonts_token'])){
			$webfonts_token=explode("--",trim($_POST['webfonts_token']));
			variable_set('webfonts_public_key',$webfonts_token[0]);
			variable_set('webfonts_private_key',$webfonts_token[1]);
			$wfs_public_key = $webfonts_token[0];
			$wfs_private_key = $webfonts_token[1];
			
		}else{
			$wfs_details=getUnPass();
			$wfs_public_key = $wfs_details[1];
			$wfs_private_key = $wfs_details[2];
			}
		
		
		if(isset($_POST['op']) && $_POST['op']=='Back to home'){
			drupal_goto("admin/settings/webfonts");
			die();
		}
		//Fetching the xml data from WFS
		$apiurl = "xml/Projects/";
		if($wfs_public_key!="" && $wfs_private_key!=""){
			$wfs_api = new Services_WFS($wfs_public_key,$wfs_private_key,$apiurl);
			$xmlMsg = $wfs_api->wfs_getInfo_post();
			
			//Creating xml DOM Instance
			$xmlDomObj = new DOMDocument();
			$xmlDomObj->loadXML($xmlMsg);
			$Messages = $xmlDomObj->getElementsByTagName("Message");
			$Message  = $Messages->item(0)->nodeValue;
			
		}
		if($Message=="Success"){
		 	$note = $xmlDomObj->getElementsByTagName("Projects"); 
			foreach($note as $value){
				$UserIds = $value->getElementsByTagName("UserId");
				$UserId  = $UserIds->item(0)->nodeValue;
			
				$UserRoles = $value->getElementsByTagName("UserRole");
				$UserRole  = $UserRoles->item(0)->nodeValue;
			}
		 
			variable_set('webfonts_userid',$UserId);
			variable_set('webfonts_usertype',(strtolower($UserRole)=="free")?0:1);
			$message='Login successful, Welcome back';
			$msg_type='status';
			$gotourl="admin/settings/webfonts/projectlist";
			drupal_set_message($message,$msg_type);
			drupal_goto($gotourl);	
			die();
			
		}else{
			$message=$Message;
			$msg_type='error';
			drupal_set_message($message,$msg_type);
			$gotourl="admin/settings/webfonts/1";
			drupal_goto($gotourl);	
		}
}


/*
* Login form generation for webfonts
*/
function GetLoginForm(){
	drupal_add_css(drupal_get_path('module', 'webfonts') .'/webfonts.css');
	$form = array();
	//Display the link of home and my account on top right 
	$form[]=array(
	 '#prefix' => '<div style="text-align:right">',
	 '#value'  => l(t(Home),"admin/settings/webfonts/projectlist")." | ".l(t('My WFS Account'),"admin/settings/webfonts/1"),
	 '#suffix' => '</div>',
	);
	
	$form['webfonts_login'] = array(
	 '#type' => 'fieldset',
	 '#title' => t('Web Fonts Info'),
	);
		
	$form['#action'] = url('admin/settings/webfonts');

	$form['webfonts_login']['webfonts_partition']=array(
	 '#prefix' => '<div style="background-color:#F1F1F1;">',
	 '#value' => t('You must be the member of <a href="'.SIGNUPURI.'" target="_blank" title="webfonts.fonts.com">webfonts.fonts.com</a> to use the plugin. If you have not registered yet, please <a href="'.SIGNUPURI.'" target="_blank" title="webfonts.fonts.com">sign up</a> here.'),
	 '#suffix' => '</div>'
	);
	
	$form['webfonts_login']['webfonts_token']=array(
	 '#type' => 'textfield',
	 '#title' => t('Authenticate key token'),
	 '#default_value' =>  variable_get('webfonts_public_key','')."--".variable_get('webfonts_private_key',''),
	 '#prefix' => '<div id="token_div" >',
	 '#suffix' => '</div><div style="margin-bottom:10px;"><a href="'.SIGNUPURI.'" target="_blank" title="webfonts.fonts.com">Sign up</a> | <a href="'.GETKEYURI.'" target="_blank" title="webfonts.fonts.com">Get authentication token key</a></div>', 
	 '#size' => 125
	);
		
	$form['webfonts_login']['submit'] = array(
	 '#type' => 'submit',
	 '#value' => t('Save Configuration'),
	 '#prefix' =>'<span class="wfs_b_o"><span class="wfs_b_i">',
	 '#suffix' =>'</span></span>',
	 '#attributes' => array('class' => 'wfs_b_s'),
	);
		
	$form['return_url'] = array(
	 '#type' => 'hidden',
	 '#default_value' => 'admin/settings/webfonts/1',
	);
	
	$form['webfonts_login']['buttons']['reset'] = array(
	 '#type' => 'button',
	 '#value' => t('Reset Configuration'),
	 '#prefix' =>'<span class="wfs_b_o wfs_ml"><span class="wfs_b_i">',
	 '#suffix' =>'</span></span>',
	 '#attributes' => array('class' => 'wfs_b_s'),
	);
		
	$form['webfonts_login']['buttons']['wfsback'] = array(
	 '#type' => 'button',
	 '#value' => t('Back to home'),
	 '#prefix' =>'<span class="wfs_b_o wfs_ml"><span class="wfs_b_i">',
	 '#suffix' =>'</span></span>',
	 '#attributes' => array('class' => 'wfs_b_s'),
	);
		
	return $form;

	}
 
 /*
 *Project Listing page from drupal datbase
 *Importing project from webfonts xml
 */
function webfonts_admin_config(&$form_state){
	drupal_add_js("$(document).ready(function() {showWFSlogo()});", 'inline');
	//Adding webfonts module css
	drupal_add_css(drupal_get_path('module', 'webfonts') .'/webfonts.css');
  
   //adding the javascript
   drupal_add_js('$(document).ready(function(){
  			
			//More action buttons event
			$("#edit-activate,#edit-deactivate,#edit-delete").click(function(){
			//checking if the checkboxes are checked or not
			if($(".plist:checked").length == 0){
				alert("Please select the project(s)");
				$("#edit-more-action").val("-");
				return false;
				}
			
			});
		
		
			//checking if the projects are selected or not before adding projects
			$("#edit-addProj").click(function(){
			if($(".importClass:checked").length == 0){
				alert("Please select the project to import.");
				return false;
				}
				}); 
			
			//DisplayHide the dropdown on more action hover
			 $(".wfs_action").mouseover(function(){
     				$("#wfs_btn_c").show();
   				 }).mouseout(function(){
			 		 $("#wfs_btn_c").hide();
  			  });
			 
			 
			  $("#selectAll").click(function(){
			 	 	$(".plist").attr("checked","true");
			  		$(".selectAllCheckbox").attr("checked","true");
			  		$(".selectAllCheckbox").val(0)
			  	});
			  
			  $("#selectNone").click(function(){
			  		$(".plist").removeAttr("checked");
			  		$(".selectAllCheckbox").removeAttr("checked")
			  		$(".selectAllCheckbox").val(1);
			  	});
			 
			
			$(".selectAllCheckbox").click(function(){
				if(  $(".selectAllCheckbox").val() == 1){
						$(".plist").attr("checked","true");
						$(".selectAllCheckbox").val(0);
			  		}else {
				  		 $(".plist").removeAttr("checked");
				  	 $(".selectAllCheckbox").val(1);
				   }
			  });
			  
			 			 
		});',
'inline');
   
    // Get the user information.
	$wfs_details=getUnPass();

  	// define number of results per page
  	$number_results =10;
	 //query for the project list from databse	
 	$sql = "SELECT project_name,wfs_configure_id,is_active FROM `".TBLCONFIG."` where user_id = '".$wfs_details[0]."'";
  	// define sql to fetch number of results
  	$sql_count = "select count(*) from `".TBLCONFIG."` where user_id = '".$wfs_details[0]."' ";
 	// execute query
 	 $resource = pager_query($sql, $number_results, 0,$sql_count);
    //$result = db_query(db_rewrite_sql($resource));
	global $base_url;
	while ($data = db_fetch_object($resource)) {
		
		$status = ($data->is_active == 1)?"tick":"cross";
		//storing information in array
		$rows[] = array(
				'checkbox' => '<input class="plist" type = "checkbox" name="project_checkbox[]" value="'.check_plain($data->wfs_configure_id).'"  >',
				'name' => l(t($data->project_name),'admin/settings/webfonts/projectdetails/'.$data->wfs_configure_id),
      			'status' => '<img src="'.$base_url.'/'.drupal_get_path('module', 'webfonts').'/images/'.t($status).'.gif"  />',
      			);
		}
	
	//If there is no project list then display the message
	 if (empty($rows)) {
    	$rows[] = array(array('data' => '<div style="text-align:center">'.t('No projects available.').'</div>', 'colspan' => '4'));
		$flag_var=1;
		}	
		
	$header = array( '<input type="checkbox" class="selectAllCheckbox" value="1">','Project Name','Active');
	$output = theme('table', $header, $rows, array('id' => 'webfonts'));
	$output .= theme('pager', NULL, $number_results, 0);
	
	//Display the link of home and my account on top right 
	$form[]=array(
		'#prefix' => '<div style="text-align:right">',
		'#value'  => l(t(Home),"admin/settings/webfonts/projectlist")." | ".l(t('My WFS Account'),"admin/settings/webfonts/1"),
		'#suffix' => '</div>',
	);
	
		
   // create a fieldset for My Project list
 	$form['project_list'] = array(
    	'#type' => 'fieldset',
    	'#title' => t('My Project List'),
    	'#collapsible' => TRUE,
    	'#collapsed' => FALSE,
  	);    
  
  	$form['project_list']['action_div']= array(
		'#prefix' => '<div class="wfs_action_main">',
	 	'#suffix' => '</div>',
	);
	
	//setting up the form action page
	$form['#action'] = url('admin/settings/webfonts/update');
		$form['project_list']['action_div'][]= array(
	'#value' => '<div class="wfs_select">Select: <a href="javascript:;" id="selectAll">All</a>, <a href="javascript:;" id="selectNone">None</a></div>',
	);
	
	$form['project_list']['action_div']['more_action'] = array(
		'#prefix' => '<div class="wfs_action">',
    	'#value' => '<a href="javascript:;" class="wfs_action_a"><span id="wfs_span">More Actions</span></a>
		<div id="wfs_btn_c" >
		<ul >
		<li><input type="submit" id="edit-delete" name="op" class="wfs_action_button" value="Delete"  /></li>
		<li><input type="submit" id="edit-activate" name="op" class="wfs_action_button" value="Activate" /></li>
		<li><input type="submit" id="edit-deactivate" name="op" class="wfs_action_button" value="Deactivate" /></li>
		</ul>
	</div>',
   	 	'#suffix' => '</div><span class="wfs_b_o" style="margin-left:10px"><span class="wfs_b_i"><input type="submit" id="edit-sync" name="op" class="wfs_action_button wfs_b_s" value="Sync" /></span></span><div class="wfs_clear"></div>',
	);
  
  //crate a table and display the data fetched from database 
	$form['project_list']['table'] = array(
    	'#prefix' => '<div>',
    	'#value' => $output,
    	'#suffix' => '</div>',
  	); 

	
	//Making the Import Project fieldset
	$form['projectList'] = array(
    	'#type' => 'fieldset',
    	'#title' => t('Import Projects'),
    	'#collapsible' => TRUE,
		'#collapsed' => TRUE
	  ); 
	
	$form['projectList']['checkboxes1'] = array(
    '#prefix' => '<div id="checkboxesaaaa">',
	 '#suffix' => '</div>',
    ); 
	
	$row=array();
	//Fetching the xml data from WFS
	$apiurl = "xml/Projects/?wfspstart=0&wfsplimit=".PROJECT_LIMIT;;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//Creating xml DOM Instance
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	
	$messages = $doc->getElementsByTagName( "Message" );
	$message = $messages->item(0)->nodeValue;
	if($message == "Success"){
	//fetching XML data
	$projects = $doc->getElementsByTagName( "Project" );
	foreach( $projects as $project )
	{
		$projectNames = $project->getElementsByTagName("ProjectName");	
		$projectName = $projectNames->item(0)->nodeValue;
	
		$projectKeys = $project->getElementsByTagName("ProjectKey");	
		 $projectKey = $projectKeys->item(0)->nodeValue;
		
		$webfonts_added_project =   webfonts_project_profile_load($projectKey, "project_key");
		 $webfonts_added_project->project_key;
		if(empty($webfonts_added_project->project_key))
			{
			
			$row[$projectKey]=check_plain($projectName);
			$output1.= '<div class="form-item" id="edit-projects-'.$projectKey.'-wrapper" style="margin:0;">
 <label class="option" for="edit-projects-'.$projectKey.'"><input type="checkbox"  value="'.$projectKey.'"  name="projects['.$projectKey.']" id="edit-projects-'.$projectKey.'" class="form-checkbox importClass">'. 	$projectName.'</label><input type="hidden" name="id_'.$projectKey.'" value="'.$projectName.'" /></div>'; 	
			$proj_id[]=$webfonts_added_project->project_key;
			} else{
			$row[$projectKey]=check_plain($projectName).'(Projects already added)';
			$output1.= '<div class="form-item" id="edit-projects-'.$projectKey.'-wrapper" style="margin:0;">
 <label class="option" for="edit-projects-'.$projectKey.'"><input type="checkbox" disabled="disabled"  value="'.$projectKey.'"  name="projects['.$projectKey.']" id="edit-projects-'.$projectKey.'" class="form-checkbox importClass">'.$projectName.' <i style="font-size:10px;color:#3392CF;">(Projects already added)</i> </label><input type="hidden" name="id_'.$projectKey.'" value="'.$projectName.'" /></div>'; 
	 		$proj_id[]=$webfonts_added_project->project_key;
				}//end of else
			
			}//end of foreach
	}//end of if for xml success
	
	//if non of the projects has been addeds
	if(empty($proj_id)){
	 $proj_id=array();
	 }
	 
	//if the projects has not been added then show the checkboxes else display the message
	if (!empty($row)) {
		//displaying the checkboxes of the project listed
    	$form['projectList']['checkboxes1']['projects'] = array(
  		'#value' => $output1,
		'#prefix' => '<div id="checkboxes">',
	 	'#suffix' => '</div>',
	);
		
	$form['projectList']['checkboxes1']['projects']['#attributes'] = array('class' => 'importClass');	
		
	//Ahah based ajax call for refreshing project list from API.	
	$projects = $doc->getElementsByTagName( "Projects" );
	foreach( $projects as $project )
	{
		$totalrecords = $doc->getElementsByTagName( "TotalRecords" );
		$totalRecord =$totalrecords->item(0)->nodeValue;
		
		$pagestarts = $doc->getElementsByTagName( "PageStart" );
		$pageStart =$pagestarts->item(0)->nodeValue;
		
		$pagelimits = $doc->getElementsByTagName( "PageLimit" );
		$pageLimit =$pagelimits->item(0)->nodeValue;
		
	}
	$wfs_pg_projects = "&nbsp;";
	if($totalRecord!="" && $pageLimit!="" && !empty($row)){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,'checkboxes','project_pagination_div',"?q=admin/settings/webfonts/projects");
		$wfs_pg_projects =  $wfs_pg->getPagination();
		}
	$form['projectList']['project_pagination']=array(
	'#prefix' => '<div class="pagination_div" id="project_pagination_div">',
	'#value' => $wfs_pg_projects,
	'#suffix' => '</div><input type="hidden" id="prj_page_start" value="'.$pageStart.'" /><input type="hidden" id="prj_page_limit" value="'.$pageLimit.'" /><input type="hidden" id="prj_total_record" value="'.$totalRecord.'" /> '
 	);
	
	}else{
		$errMsg = ($message == "Success")?"No projects available":$message;
		$form['projectList']['noprojects'] = array(
			'#value' => $errMsg,
			'#type' => 'item',
			'#prefix' => '<div id="checkboxes">',
	 '#suffix' => '</div>',
		);
	}
	$form['projectList']['refresh'] = array(
    '#value' =>'Refresh List',
    '#type' => 'submit',
	'#prefix' =>'<span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span>', 
	 );
	$form['projectList']['refresh'] ['#attributes']=array('class' => 'wfs_b_s');
	if (!empty($row)) {	$display = 'block'; } else { $display = 'none'; }
	//Adding a add project button 
	$form['projectList']['addProj'] = array(
  	'#prefix' =>'<span class="wfs_b_o" id="addbutton" style="display:'.$display.'"><span class="wfs_b_i">',
	'#suffix' =>'</span></span>',
	'#type' => 'submit',
  	'#value' => t('Add Project'),
	
	);
	
	$form['projectList']['addProj'] ['#attributes']=array('class' => 'wfs_b_s');
	
  // return the form
  return $form;
}


/**
*Ajax calling function form refreshing project list
*Listing the xml file of project lists from ajax call
*/
function webfonts_project_list(){
	$row=array();
 	//get user information
	$wfs_details=getUnPass();
	$startPage = (isset($_GET['pageStart']))?$_GET['pageStart']:0;
	//Fetching the xml data from WFS
	$apiurl = "xml/Projects/?wfspstart=".$startPage."&wfsplimit=".PROJECT_LIMIT;;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//Creating xml DOM Instance
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	
	$messages = $doc->getElementsByTagName( "Message" );
	$message = $messages->item(0)->nodeValue;
	if($message == "Success"){
	
	$output = '<div class="form-checkboxes">';
	//fetching XML data
	$projects = $doc->getElementsByTagName( "Project" );
	foreach( $projects as $project )
		{
		$projectNames = $project->getElementsByTagName("ProjectName");	
		$projectName = $projectNames->item(0)->nodeValue;
	
		$projectKeys = $project->getElementsByTagName("ProjectKey");	
		 $projectKey = $projectKeys->item(0)->nodeValue;
		
		$webfonts_added_project =   webfonts_project_profile_load($projectKey, "project_key");
		 $webfonts_added_project->project_key;
		if(empty($webfonts_added_project->project_key))
			{
			$checkboxNumber =1;
			$output.= '<div class="form-item" id="edit-projects-'.$projectKey.'-wrapper">
 <label class="option" for="edit-projects-'.$projectKey.'"><input type="checkbox"  value="'.$projectKey.'"  name="projects['.$projectKey.']" id="edit-projects-'.$projectKey.'" class="form-checkbox importClass">'. 	$projectName.'</label><input type="hidden" name="id_'.$projectKey.'" value="'.$projectName.'" /></div>'; 	
 			}
			 else{
				
			$output.= '<div class="form-item" id="edit-projects-'.$projectKey.'-wrapper">
 <label class="option" for="edit-projects-'.$projectKey.'"><input type="checkbox" disabled="disabled"  value="'.$projectKey.'"  name="projects['.$projectKey.']" id="edit-projects-'.$projectKey.'" class="form-checkbox importClass">'.$projectName.' <i style="font-size:10px;color:#3392CF;">(Projects already added)</i> </label><input type="hidden" name="id_'.$projectKey.'" value="'.$projectName.'" /></div>'; 				
				}
		} 
		$projects = $doc->getElementsByTagName( "Projects" );
		foreach( $projects as $project )
		{
			$totalrecords = $doc->getElementsByTagName( "TotalRecords" );
			$totalRecordxml =$totalrecords->item(0)->nodeValue;
		
			$pagestarts = $doc->getElementsByTagName( "PageStart" );
			$pageStartxml =$pagestarts->item(0)->nodeValue;
		
			$pagelimits = $doc->getElementsByTagName( "PageLimit" );
			$pageLimitxml =$pagelimits->item(0)->nodeValue;
		
		}
		$pageStart = (isset($_GET['pageStart']))?$_GET['pageStart']:$pageStartxml;
		$pageLimit =(isset($_GET['pageLimit']))?$_GET['pageLimit']:$pageLimitxml;
		$totalRecord = (isset($_GET['totalRecords']))?$_GET['totalRecords']:$totalRecordxml ;
		$contentDiv = $_GET['contentDiv'];
		$paginationDiv = $_GET['paginationDiv'];
		$pagination ="&nbsp;";
		if($totalRecord!=0 && $pageLimit!="" && !empty($output)){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,$contentDiv,$paginationDiv,"?q=admin/settings/webfonts/projects");
		$pagination = $wfs_pg->getPagination();
		}
 	}
	
		if(empty($output))
		{
		$output = $message;
		}
	

  // Final rendering callback.
  print drupal_json(array('status' => TRUE, 'data' => $output,'dataNo' => $checkboxNumber,'pagination'=>$pagination));
 }

/**
*Project form submitting function
* Actions: activate/deactivate/delete/sync/add project
*/
function webfonts_project_status_update(&$form_state){
	//Action to be perfomed
	$case = $form_state[post][op];

	switch ($case){
	case 'Activate':
	case 'Deactivate':
		
	//perform task for activate or deactivate
	$projects = $form_state[post][project_checkbox];

	if(count($projects)!=0){
		foreach($projects as $key => $project)
		{
			if($form_state[post][op] == 'Activate')
			{
			db_query("UPDATE `".TBLCONFIG."` SET is_active = '1', updated_date = NOW() where wfs_configure_id = '%d'",$project);
			}else{
			db_query("UPDATE `".TBLCONFIG."` SET is_active = '0', updated_date = NOW() where wfs_configure_id = '%d'",$project);
			}
		}
	drupal_set_message(t('Projects updated successfully.'));
		}
	//end of activate/deactivate action 
	break;
		
	case 'Add Project':
	//Adding projects to drupal database
	$webfonts_projects_added='';
	$webfonts_projects_already_added='';
	$count=1;
	$count_added=1;
	
	if(isset($_POST['projects'])){
	foreach($_POST['projects'] as $key=>$val)
	{
	
	$id = webfonts_project_profile_load($val, "project_key");
	
	if(empty($id->project_key)){
		$wfs_details=getUnPass();
		if(strtolower($wfs_details[3]) == 0)
			{$userid = 0;}else{$userid = 1;}
			db_query("INSERT INTO `".TBLCONFIG."` (project_name,project_key,project_day,user_id,user_type,updated_date) VALUES('%s','%s','0-6','%s','%d', NOW())", $_POST['id_'.$key],$val,$wfs_details[0],$userid);
			$webfonts_projects_added=$_POST['id_'.$key]." ";
			$count++;
		}else{
			$webfonts_projects_already_added=$_POST['id_'.$key]." ";
			$count_added++;
	}
	if($count > 1)
		{
		drupal_set_message(t('Project %project has been added successfully.', array('%project' => $webfonts_projects_added)));
		$count--;
		}
	if($count_added >1)
		{
		drupal_set_message(t('Project %project has already been added.', array('%project' => $webfonts_projects_already_added)),'warning');
		$count_added--;
	
		}
	}//end of foreach
	
	}//end of if
	break;
	
	case 'Sync':
	//get user information
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Projects/";
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//creating a DOM object
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	//check the message status
	$messages = $doc->getElementsByTagName( "Message" );
	$message = $messages->item(0)->nodeValue;
	if($message == "Success") {
	//fetching XML data
	$projects = $doc->getElementsByTagName( "Project" );
	foreach( $projects as $project )
	{
		$projectNames = $project->getElementsByTagName("ProjectName");	
		$projectName = $projectNames->item(0)->nodeValue;
	
		$projectKeys = $project->getElementsByTagName("ProjectKey");	
		$projectKey = $projectKeys->item(0)->nodeValue;
	
		//update the projects that are database
		$webfonts_added_project =   webfonts_project_profile_load($projectKey, "project_key");
		if(!empty($webfonts_added_project->project_key))
			{
		
				db_query("UPDATE `".TBLCONFIG."` SET project_name = '%s', project_key = '%s', updated_date = NOW() WHERE wfs_configure_id = '%d'",$projectName, $projectKey, $webfonts_added_project->wfs_configure_id);
			} 
		
		}
	}
	//Delete all the projects that are not in the user accounts
	$xmlData = $xmlUrl;
	$query = "SELECT * FROM `".TBLCONFIG."`";
	$rs = db_query($query);
 	
	if ($rs) {
		while ($data = db_fetch_object($rs)) {
			preg_match("/<ProjectKey>".$data->project_key."/", $xmlData, $matches);
			if(empty($matches[0]))
			{
				db_query("DELETE FROM `".TBLCONFIG."` WHERE project_key = '%s'", $data->project_key);
			}
		}
	}

	drupal_set_message(t('Projects synchronized successfully.'));
	//end of sync
	
	break;
	
	case 'Delete':
	$projects = $form_state[post][project_checkbox];
	$deleteIds = implode(",", $projects);
	db_query("DELETE FROM `".TBLCONFIG."` WHERE wfs_configure_id IN (%s)",$deleteIds);
		
	default:
		break;
	}
//redirect to webfonts
drupal_goto("admin/settings/webfonts");
}


/*
**load the individual projects details
@project id int
@key: condition to be searched
*/
function webfonts_project_profile_load($project_id, $key="wfs_configure_id") {
	$query = "SELECT * FROM `".TBLCONFIG."` WHERE ".$key." = '%s'";
	$rs = db_query($query, $project_id);
 
	if ($rs) {
		while ($data = db_fetch_object($rs)) {
			return $data;
		}
	}
 	return FALSE;
}

/*
** Add the tab and bread crumb in project list page
*/
function webfonts_tab($form_id,$projectid) {
	//Adding the webfonts cookie plugin
	drupal_add_js(drupal_get_path('module', 'webfonts') .'/jquery_cookie.js');
	//Adding the webfonts Css files
	drupal_add_css(drupal_get_path('module', 'webfonts') .'/webfonts.css');
	//Adding a inline Javascript 
	drupal_add_js('$(document).ready(function(){

			
		  var tabContainers = $("div.wfs_tabs > div");
                        tabContainers.hide().filter(":first").show();
                        
                        $("div.wfs_tabs ul.tabNavigation a").click(function () {
                                tabContainers.hide();
                                tabContainers.filter(this.hash).show();
                                $("div.wfs_tabs ul.tabNavigation a").removeClass("selected");
                                $(this).addClass("selected");
				$.cookie("wfstabindex", this.id);
                                return false;
                        });
						
						function wfscookiefn(button){
							tabContainers.hide();
							tabContainers.filter(button.hash).show();
							$("div.wfs_tabs ul.tabNavigation a").removeClass("selected");
							$(button).addClass("selected");
							return false;
						}
						if($.cookie("wfstabindex")){
								wfscookiefn(document.getElementById($.cookie("wfstabindex")));
							
								
						}else{
							wfscookiefn(document.getElementById("wfs_configure"));
						}
		              
				 
						 $("ul.tabNavigation1 a").click(function () {
						 $("#selectors").hide()
						 $("#fonts").hide()
						 $(this.hash).show();
						 $("ul.tabNavigation1 a").removeClass("selected");
						 $(this).addClass("selected");
                             
							   return false;
                        });
						
						
		$("#showPage").click(function(){
			var str = (window.location).toString();
			var loc = str.replace(/projectdetails/gi,"pages");
			window.showModalDialog(loc,"Project List","dialogWidth:800px; dialogHeight:800px; center:on; resizable:yes");
			return false;
			});
		
		$("#edit-enable-editor").change(function(){
			if(	$("#edit-enable-editor").attr("checked")){
				var pid = $("#edit-userid").val();
				$("#enableeditorshowload").html("<img src=\""+fullpath+"/modules/webfonts/images/loading.gif\"  />");		
				$.ajax({
			   		type: "GET",
					url: "?q=admin/settings/webfonts/ajaxeditor",
					data: {
       					 "project_id" : pid
        				 },
					dataType: "json",  
   					success: function(data){
						
						if(data.status == 1)
							{
							alert(data.message);
								$("#enableeditorshowload").html("");		
							}
						else { 
							$("#enableeditorshowload").html("");
							return true;
							 }
   					},
					error: function(msg){
						alert("An error occured: " + msg.status);
						}
  				 }); 
			} 
		});
	});','inline'); 

  
	$form['#prefix']= '<div class="wfs_tabs">';

	$form['error_msg'] = array(
	'#prefix' => '<div id="errMsg" class="messages error" style="display:none;background-color:#FFCCCC;padding:2px;">',
	'#value' => '&nbsp',
	'#suffix' => '</div>',
	);
	//Display the link of home and my account on top right 
	$form[]=array(
	'#prefix' => '<div style="text-align:right;margin-bottom:10px;">',
	'#value'  => l(t(Home),"admin/settings/webfonts")." | ".l(t('My WFS Account'),"admin/settings/webfonts/1"),
	'#suffix' => '</div>',
	);
	
	$form[]=array(
	'#prefix' => '<ul class="tabNavigation">',
	'#value'  => '<li><a id="wfs_configure" href="#first">Configure</a></li>
            		<li><a id="wfs_wos" href="#second">Work on stylesheet</a></li>
            		<li><a id="wfs_domain" href="#third">Domain</a></li>',
	'#suffix' => '</ul>',
	);
	
	
	$projectDetails = webfonts_project_profile_load($projectid, $key="wfs_configure_id");
	//Display the Breadcrumb 
	$form[]=array(
	'#prefix' => '<div class="wfs_breadcrumb">',
	'#value'  => l(t('My Project'),"admin/settings/webfonts").'>>'.'<span>'.$projectDetails->project_name.'</span>',
	'#suffix' => '</div>',
	);
	
	$form['#attributes'] = array('class' => 'wfs_tab_form');
	return $form;
	
}

/**
* Configure tab form Generation Page
*form id: name of the hook
*project id: project id of drupal database
*/
function webfonts_configure($form_id, $projectid) {

	//Adding the javascript for configure tab
	drupal_add_js('$(document).ready(function(){
			$(".pagecheckbox").click(function(){
				$("#edit-pageHidden").val(\'\');
				$("#edit-pageNameHidden").val(\'\');
				$(".pagecheckbox[@checked]").each(
 				 function() {
					
					 var newval = $("#edit-pageHidden").val();
   					$("#edit-pageHidden").val(this.value+\'@\'+newval);
					$("#edit-pageNameHidden").val($("#edit-page-name"+this.value).val()+\'@\'+$("#edit-pageNameHidden").val());
		 	});
				
			//for the node url
				var newval = $("#edit-pageHidden").val();
				var strLen = newval.length;
				var newstr = newval.slice(0,strLen-1) 
				var pagelist = newstr.split("@");
				var storedPages = document.getElementById(\'edit-webfonts-visibility-pages\').value;
				var pages = "";
				
				for(var i=0;i<pagelist.length;i++)
					{
					pages = pages + "node/"+pagelist[i];
					if(i != (pagelist.length-1))	
						{
						pages = pages +"\n";
						}  
					}
	
			if(pages!="" || $(".pagecheckbox:checked").length == 0){			
				document.getElementById(\'edit-webfonts-visibility-pages\').value =  pages;
				}
			});
				
		
			//For third radio button option
			$(".radiocheck").click(function(){
				if($(this).find("input[type=\'radio\']:checked").val() == 2)
					{
					$("#pageaddField").hide();
					$("#textareaField").show();
					}else{
					$("#pageaddField").show();
					$("#textareaField").hide();;
					}
				});
			});',
'inline');

	$php_access = user_access('use PHP for block visibility');
	
	$visibility_options = array(
    0 => t('Show on every page except listed page, leave blank to show on all pages.'),
    1 => t('Show on only the listed pages .'),
  	  );
    
	// Check if use has access to do PHP stuff
  	if ($php_access) {
    $visibility_options[2] = t('Show if the following php code returns true (expert mode).');
    $visibility_desc = ' ' . t('For expert mode include %php to enclose php code.', array('%php' => '<?php ?>'));
  	}
  	//fetching the project details
	$projectDetails = webfonts_project_profile_load($projectid, $key="wfs_configure_id");

	$form['general_information'] = array(
	 '#prefix' => '<div class="wfs_day_row">',
	  '#value' =>  t('You can select the pages to display web fonts, please make sure your Web Font Project has the fonts selected and has the corresponding Selectors.'),
	  '#suffix' => '</div>',
	);
	
	$form['webfonts_visibility'] = array(
	'#type' => 'fieldset',
	'#title' => '<strong>'.t('Pages Display Setting').'</strong>',
	);
	
	 $form['webfonts_visibility']['webfonts_visibility_settings']['custom'] = array(
    '#type' => 'radios',
    '#options' => $visibility_options,
   	'#default_value' => $projectDetails->project_page_option,
	'#attributes' => array('class' => 'radiocheck')
  	);
	
	 $form['webfonts_visibility']['webfonts_visibility_pages'] = array(
    '#type' => 'textarea',
	'#prefix' => '<div style="display:none">',
    '#default_value' => $projectDetails->project_pages, 
	 '#suffix' => '</div>',
	  	);
	
	//fetching the node title
	$node_title="";
	$node_list = explode("\n", $projectDetails->project_pages);
	if(count($node_list)>0)
	{
		foreach ($node_list as $node)
		{
			$node_id = explode("/",$node);
			$node = node_load($node_id[1]*1);
			$node_title.= ($node_title!='')?"\n":"";
			$node_title.= $node->title;
		}
	} 
	/*adding pages code*/
	$data = webfonts_project_profile_load($projectid);
	$project_pages = $data->project_pages;
	$pages = explode("\n",$project_pages);
	$pageid = array();
	foreach ($pages as $key=>$page)
	{
	 $pageids = explode("/",$page);
	 array_push($pageid,$pageids[1]);
	}
	
	$query = "SELECT * FROM {node} WHERE status ='1'";
	$rsa = db_query($query, $project_id);
 	$rowdata=array();
	if ($rsa) {
		while ($data1 = db_fetch_object($rsa)) {
			if (in_array($data1->nid*1, $pageid)) {
			$rowdata[]=$data1->nid;
			 }
			$row[$data1->nid]=check_plain($data1->title);
			$form['page_name'.$data1->nid] = array(
  			'#type' => 'hidden',
			'#value' => $data1->title,
		);
		}
	}
	if($projectDetails->project_page_option == 2) {
		$display = "display:none";
		}
	
	//Making the Project list fieldset
	$form['webfonts_visibility']['pageList'] = array(
    '#type' => 'fieldset',
    '#title' => t('Page List'),
	'#collapsible' => TRUE,
    '#collapsed' => FALSE,
	'#attributes' => array('id' => 'pageaddField','style' => $display)
     );
	
	//if the page has  been added then show the checkboxes 
	if (!empty($row)) {
		//displaying the checkboxes of the page listed
    	$form['webfonts_visibility']['pageList']['page'] = array(
  		'#type' => 'checkboxes',
		'#options' => $row,
		'#default_value' =>$rowdata,
		);
	$form['webfonts_visibility']['pageList']['page']["#attributes"] = array('class' => 'pagecheckbox');
	}
	
	if(empty($row))
	{
		$form['webfonts_visibility']['pageList']['nopage'] = array(
		'#prefix' => '<div>',
		'#value' => t('All pages has been added.'),
		'#suffix' => '</div>',
		);
		
	}
	$form['pageHidden'] = array(
	'#type' => 'hidden',
	);
	
	$form['pageNameHidden'] = array(
	'#type' => 'hidden',
	); 
	
	$textareaDisplay = ($projectDetails->project_page_option != 2)?"display:none":"display:block";
	
	//end of page adding 
	$form['webfonts_visibility']['webfonts_textarea_fieldset'] = array(
	'#type' => 'fieldset',
	'#attributes' => array('id' => 'textareaField','style' => $textareaDisplay)
	);
	
	$pageShowContent = ($projectDetails->project_page_option == 2)?$projectDetails->project_pages:"";
	$form['webfonts_visibility']['webfonts_textarea_fieldset']['webfonts_visibility_pages_show'] = array(
    '#type' => 'textarea',
    '#default_value' => $pageShowContent,
    '#description' => $visibility_desc,
	);
	
  $form['webfonts_visibility']['webfonts_display_day'] = array(
  '#type' => 'textfield',
  '#prefix' => '<div class="wfs_day_row">'.t("Display for day of week.  0 for Sunday, 1 for Monday...and 6 for Saturday, use - for day range {eg: 0-6 for displaying from Sunday to Saturday}, you can also use comma separate days{eg: 1,3,4 for displaying on Monday, Wednesday and Thrusday}"),
  '#default_value' => $projectDetails->project_day ,
  '#size' => 30,
  '#required' => FALSE,
  '#suffix' => '</div>',
	);
	$enable_subsetting = 0;
	//checking if there is any subsetting true
	$fontsArray = getPrjFonts($projectDetails->project_key);
	if(is_array($fontsArray['EnableSubsetting'])){
		if (in_array('true', $fontsArray['EnableSubsetting'])){
		$enable_subsetting = 1;
		}
	}
	//display the options to use for level one user [options: stylesheet/javascript]
	if($data->user_type == 1){
		$form['webfonts_styleorjs_option'] = array(
 		'#type' => 'fieldset', 
	);
		 
	$form['webfonts_styleorjs_option']['selection_option'] = array(
 	'#prefix' => '<div class="wfs_blue_header">',
	'#title' => 'Please select the options to be used',
	'#suffix' => '</div>',
	'#default_value' => $data->project_options,
 	'#options' => array(t('Javascript'), t('Stylesheet')),
  	'#type' => 'radios',
	);

	}
	
	/****** Commented to disable the editor option - By Keshant ******/
	
	/*$form['webfonts_editor_option'] = array(
 	'#type' => 'fieldset', 
	);
	
	$form['webfonts_editor_option']['editor_title'] = array(
 	'#prefix' => '<div class="wfs_blue_header">',
	'#value' => 'Webfonts Editor Option',
	'#suffix' => '</div>'
	);
	$form['webfonts_editor_option']['editor_select'] = array(
 	'#prefix' => '<div class="wfs_blue_header" style="color:#000;">',
	'#title' => 'Please select the editor to be used',
	'#suffix' => '</div>',
	'#default_value' => $data->editor_select,
 	'#options' => array('TinyMCE','Ckeditor'),
  	'#type' => 'radios',
	);
		$form['webfonts_editor_option']['editor_message'] = array(
 	'#prefix' => '<div class="description">',
	'#value' => 'Please deactivate other module that replaces editor in the drupal before enabling the editor.',
	'#suffix' => '</div>'
	);
	
	$form['webfonts_editor_option']['enable_editor'] = array(
  	'#type' => 'checkbox',
  	'#title' => t('Enable the online editor with web fonts from your Web Font Project.').'<span id="enableeditorshowload"></span>',
  	'#default_value' => $projectDetails->wysiwyg_enabled,
	);
	
	$form['webfonts_editor_option'][] = array(
    '#value' => '<a href="javascript:;" id="editorShow">See online editor with web fonts</a><span id="editorshowload"></span>',
	);
	 drupal_set_html_head('<script type="text/javascript" src="'.GETFONTFAMILIESURI.$projectDetails->project_key.'"></script>');
	 drupal_add_js(drupal_get_path('module', 'webfonts') . '/sample.js');

	//IFRAME content
	$form['webfonts_editor_option']['textarea_div'] = array(
	'#prefix' => '<div id="webfontsEditor" style="display:none">', 
	'#value' => '<div class="wfs_blue_header">TinyMCE Sample: </div><textarea id="tinymce_sample" name="tinymce_sample" style="width:100%;height:100%;"></textarea><div class="wfs_blue_header">Ckeditor Sample: </div>',
 	'#suffix' => '</div><div id="contents" style="display: none:margin-top:10px;">
		</div>'
	);
	$form['webfonts_editor_option']['cur_project_key'] = array(
   '#type' => 'hidden',
   '#value' => $projectDetails->project_key
	);

	if($enable_subsetting == 1){
	$form['webfonts_editor_option']['textarea_div']['preview_font'] = array(
   '#type' => 'submit',
   '#value' => t('Preview'),
	'#prefix' =>'<span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span><div style="clear:both"></div>',
	);
	$form['webfonts_editor_option']['textarea_div']['preview_font']['#attributes']=array('class' => 'wfs_b_s');	
	}*/
	
	$form['hidden'] = array(
 	'#type' => 'hidden',
	'#value' => $projectid,
	);
	
	$form['userid'] = array(
 	'#type' => 'hidden',
	'#value' => $data->user_id,
	);
	
	$form['submit'] = array(
 	'#type' => 'submit',
	'#value' => t('Save'),
	'#prefix' =>'<span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span>',
	 );
	$form['submit']['#attributes']=array('class' => 'wfs_b_s');	
	$form['#prefix']= '<div id="first">';
	$form['#suffix']= '</div>';
	
	$form['redirect'] = FALSE;

	return $form;
}

/**
*Domain tab Form Generation page
*form id: name of the hook
*project id: project id of drupal database
*/
function webfonts_domain($form_id,$projectid) {

	$form['webfonts_domain_field'] = array(
 	'#type' => 'fieldset',
	'#title' => '<Strong>'.t('Domain').'</strong>',
	);
	//get project information
	$data = webfonts_project_profile_load($projectid);
	//get user information
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Domains/?wfspstart=0&wfsplimit=".DOMAIN_LIMIT."&wfspid=".$data->project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//Creating xml DOM Instance	
		$doc = new DOMDocument();
		$doc->loadXML($xmlUrl);
		
		$messages = $doc->getElementsByTagName( "Message" );
		$message = $messages->item(0)->nodeValue;
		if(strtolower($message)=="success"){
			//fetching XML data
			$domains = $doc->getElementsByTagName( "Domain" );
			foreach( $domains as $domain )
			{
				$domainNames = $domain->getElementsByTagName("DomainName");	
				$domainName = $domainNames->item(0)->nodeValue;
			
				$domainIDs = $domain->getElementsByTagName("DomainID");	
				$domainID = $domainIDs->item(0)->nodeValue;
			
			
					$rows[] = array(
						'name' => '<a href="http://'.$domainName.'" target="_blank">'.check_plain($domainName).'</a>',
						'action' => l(t('Edit'), "admin/settings/webfonts/domainedit/".$domainID."/".$projectid."/".urlencode(urlencode($domainName)))." | ".l(t('Remove'), "admin/settings/webfonts/domaindelete/".$domainID."/".$projectid."/".urlencode(urlencode($domainName))),);
			$count_domain = 1;		
			}
			
			//if non of the projects has been addeds
			if(empty($rows)){
				$rows[] = array(array('data' => '<div style="text-align:center">'.t('No Domains available.').'</div>', 'colspan' => '2'));
			 }
		}else{
			$count_domain = 1;
			$rows[] = array(array('data' => '<div style="text-align:center">'.t($message).'</div>', 'colspan' => '2'));
		}
	
	$output = theme('table', $header, $rows, array('id' => 'webfontsDomain'));
	
	 //crate a table and display the data fetched from database 
	$form['webfonts_domain_field']['table'] = array(
	'#prefix' => '<div id="domainList">',
	'#value' => $output,
	'#suffix' => '</div>',
	);

	$totalrecords = $doc->getElementsByTagName( "TotalRecords" );
	$totalRecord =$totalrecords->item(0)->nodeValue;
		
	$pagestarts = $doc->getElementsByTagName( "PageStart" );
	$pageStart =$pagestarts->item(0)->nodeValue;
		
	$pagelimits = $doc->getElementsByTagName( "PageLimit" );
	$pageLimit =$pagelimits->item(0)->nodeValue;
	$wfs_pagination_domain="&nbsp;";
	
	if($totalRecord!="" && $pageLimit!="" && $count_domain == 1){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,'domainList','domain_pagination_div',"?q=admin/settings/webfonts/domains/".$projectid);
		$wfs_pagination_domain = $wfs_pg->getPagination();
	}
	$form['webfonts_domain_field']['domain_pagination_div']=array(
	'#prefix' => '<div class="pagination_div" id="domain_pagination_div">',
	'#value' => $wfs_pagination_domain,
	'#suffix' => '</div><input type="hidden" id="domain_page_start" value="'.$pageStart.'" /><input type="hidden" id="domain_page_limit" value="'.$pageLimit.'" /><input type="hidden" id="domain_total_record" value="'.$totalRecord.'" /> '
 	);
	$form['webfonts_add_domain'] = array(
 	'#type' => 'fieldset',
	'#title' => '<strong>Add Domain</strong>'
	);
	
	$form['webfonts_add_domain']['project_key'] = array(
 	'#type' => 'hidden',
	'#value' => $projectid,
	);
	
	$form['webfonts_add_domain']['add_domain'] = array(
 	'#type' => 'textfield',
		'#prefix' =>'<div class="wfs_addselect_txt">',
	'#suffix' =>'</div>',
	);
	
	$form['webfonts_add_domain']['submit_domain'] = array(
 	'#type' => 'submit',
	'#value' => t('Add'),
	'#prefix' =>'<div class="wfs_select_btn"><span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span></div>',
	 );
	
	
	$form['webfonts_add_domain']['submit_domain']['#attributes']=array('class' => 'wfs_b_s');
		
	//please don't delete this 
	$form['#prefix']= '<div id="third">';
	$form['#suffix']= '</div>';
	
	$form['redirect'] = FALSE;
	
	return $form;

}
/**
*Selectors tab Form Generation page
*form id: name of the hook
*project id: project id of drupal database
*/
function webfonts_selectors($form_id,$projectid) {
	//adding javascript file
 	drupal_add_js('$(document).ready(function(){
				   initBinding();
					   });','inline');

	//Creating a selectors fieldset
	$form['webfonts_selectors'] = array(
	'#type' => 'fieldset',
	'#title' => '<strong>Selectors</strong>'
	);
	
	$row=array();
	//get Project infromation
	$data = webfonts_project_profile_load($projectid);
	//get user details
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Selectors/?wfspstart=0&wfsplimit=".SELECTOR_LIMIT."&wfspid=".$data->project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//Creating xml DOM Instance	
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	
	//fetching XML data
	$messages = $doc->getElementsByTagName( "Message" );
	$message = $messages->item(0)->nodeValue;
	
	$count = 1;
	$wfsSelectorTag=array();
	if($message == "Success"){
	$selectors = $doc->getElementsByTagName( "Selector" );
	foreach( $selectors as $selector ){
		$SelectorTags = $selector->getElementsByTagName("SelectorTag");	
		$SelectorTag = $SelectorTags->item(0)->nodeValue;
		
		$wfsSelectorTag[]=$SelectorTag;//array for list of selectors
		
		$SelectorIDs = $selector->getElementsByTagName("SelectorID");	
		$SelectorID = $SelectorIDs->item(0)->nodeValue;
				
		$SelectorFontIDs = $selector->getElementsByTagName("SelectorFontID");	
		$SelectorFontID = $SelectorFontIDs->item(0)->nodeValue;
		
		$fontsArr = webfonts_font_list($projectid,$SelectorFontID,$count);
		$rows[] = array(
				'tag' => '<strong>'.$SelectorTag.'</strong>',
				'fonts' => $fontsArr[0] ,
      			'sample' => '<span class="wfs_selectors" style="font-size:26px;font-family:'.$fontsArr[3].'" id="fontid_'.$count.'">'.$fontsArr[1].'</span>',
      			'action' => l(t('Remove'), "admin/settings/webfonts/selectorsdelete/".$SelectorID."/".$SelectorTag."/".$projectid)
				);
		$selectorKey.= '<input id="edit-Selector-'.$count.'" name="Selector_'.$count.'" value="'.$SelectorID.'" type="hidden" >';
		$count++;
	}
	}
	$_SESSION['webfonts_project_id']=$projectid;
	//if non of the selectors has been addeds
	if(empty($rows)){
	 $rows[] = array(array('data' => '<div style="text-align:center">'.t('No selectors available.').'</div>', 'colspan' => '4'));
	 }
	
	$output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
	$output.= $selectorKey;
		
	//crate a table and display the data fetched from selctors xml 
  	$form['webfonts_selectors']['table'] = array(
    '#prefix' => '<div id="selectorList">',
    '#value' => $output,
    '#suffix' => '</div>',
  	);

	$totalrecords = $doc->getElementsByTagName( "TotalRecords" );
	$totalRecord =$totalrecords->item(0)->nodeValue;
		
	$pagestarts = $doc->getElementsByTagName( "PageStart" );
	$pageStart =$pagestarts->item(0)->nodeValue;
	
	$pagelimits = $doc->getElementsByTagName( "PageLimit" );
	$pageLimit =$pagelimits->item(0)->nodeValue;
	$wfs_pagination_selector = "&nbsp;";
 	if($totalRecord!="" && $pageLimit!="" && $count != 1){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,'selectorList','selector_pagination_div',"?q=admin/settings/webfonts/selectors_list/".$projectid);
		$wfs_pagination_selector = $wfs_pg->getPagination();
	}
	
	$form['webfonts_selectors']['selectors_pagination_div']=array(
	'#prefix' => '<div class="pagination_div" id="selector_pagination_div">',
	'#value' => $wfs_pagination_selector,
	'#suffix' => '</div><input type="hidden" id="selector_page_start" value="'.$pageStart.'" /><input type="hidden" id="selector_page_limit" value="'.$pageLimit.'" /><input type="hidden" id="selector_total_record" value="'.$totalRecord.'" /> '
 	);
	
	$form['webfonts_add_selectors'] = array(
 	'#type' => 'fieldset',
	'#title' => '<strong>Add Selectors</strong>'
	);
	
	$form['webfonts_add_selectors']['add_selector'] = array(
 	'#type' => 'textfield',
		'#prefix' =>'<div class="wfs_addselect_txt">',
	'#suffix' =>'</div>',
	);
	$form['webfonts_add_selectors']['submit_selector'] = array(
 	'#type' => 'submit',
	'#value' => t('Add'),
	'#prefix' =>'<div class="wfs_select_btn"><span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span></div>',
	 );
	
	$form['webfonts_add_selectors']['submit_selector']['#attributes']=array('class' => 'wfs_b_s');	
	
	$form['sub_tab']=array(
	'#prefix' => '<div><ul class="tabNavigation1">',
	'#value'  => '<li><a href="#fonts" class="selected">Fonts</a></li>
	<li><a href="#selectors">Drupal existing selectors</a></li>',
     '#suffix' => '</ul></div>',
	);
	
	$form['projectid'] = array(
 	'#type' => 'hidden',
	'#value' => $projectid
	);
	
	$form['webfonts_selectors']['submit'] = array(
 	'#type' => 'submit',
	'#value' => t('Save'),
	'#prefix' =>'<div>&nbsp;</div><span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span>',
	 );
	
	$form['webfonts_selectors']['submit']['#attributes']=array('class' => 'wfs_b_s');	
	
	$project_key=$data->project_key;
	$fontsArray = getPrjFonts($project_key);
	
	$rows = array();
	$rows1 = array();
	if(count($fontsArray["FontName"])>0){
	for($i=0;$i< count($fontsArray["FontName"]);$i++){
		$rows[] = array(
		'name' => '<div class="font_sep odd ">
		<div class="font_img" style="font-family:\''.$fontsArray["FontCSSName"][$i].'\' !important;font-size:20px;">'.$fontsArray["FontPreviewTextLong"][$i].'</div>
		<div class="fontnames"><u>'.$fontsArray["FontName"][$i].'</u> | <u>'.$fontsArray["FontFondryName"][$i].'</u>
		| <u>'.$fontsArray["FontLanguage"][$i].'</u>
		'.$fontsArray["FontSize"][$i].'
		</div></div>',);
		
		
	}//end of for loop.
		}else{
		 $rows[] = array(array('data' => '<div style="text-align:center">'.t('No fonts available.').'</div>'));
		}
	
	//start for selectors
	$wfs_selector=getAllActiveSelectors();

	for($i=0;$i< count($wfs_selector);$i++){
		$rows1[] = array('name' => '<strong>'.$wfs_selector[$i].'</strong>','action' =>	'<div id="div'.urlencode($wfs_selector[$i]).'">'.(in_array($wfs_selector[$i],$wfsSelectorTag)?'Added to webfonts':'<a href="javascript:void(0);" id="'.urlencode($wfs_selector[$i]).':'.$projectid.'" class="addtowebfonts">Add to webfonts</a>').'</div>',);
	}
	$header=array();
	$output = theme("table",$header, $rows,array('id' => 'webfonts_fonts'));
	$output1 = theme("table",$header, $rows1,array('id' => 'webfonts_selectors'));
	
	$form['webfonts_fonts_field'] = array(
	'#type' => 'fieldset',
	'#title' => '<strong>Fonts</strong>',
	'#prefix'=>'<div id="fonts">',
	'#suffix'=> '</div>',
	);
	
	$form['webfonts_fonts_field']['webfonts_fonts'] = array(
	'#prefix'=>"<div id='fonts_main'>",
	'#value'=>$output,
	'#suffix'=>"</div>");
	//Pagination data
	$totalRecord = $fontsArray['TotalRecords'];
    $pageStart = $fontsArray['PageStart'];
	$pageLimit = $fontsArray['PageLimit'];
	$wfs_paginatation_fonts = "&nbsp;";
	if($totalRecord!="" && $pageLimit!="" && (count($fontsArray["FontName"])>0)){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,'fonts_main','font_pagination_div',"?q=admin/settings/webfonts/fontslists/".$project_key);
		$wfs_paginatation_fonts = $wfs_pg->getPagination();
	}
	
	$form['webfonts_fonts_field']['fonts_pagination_div']=array(
	'#prefix' => '<div class="pagination_div" id="font_pagination_div">',
	'#value' => $wfs_paginatation_fonts,
	'#suffix' => '</div><input type="hidden" id="font_page_start" value="'.$pageStart.'" /><input type="hidden" id="font_page_limit" value="'.$pageLimit.'" /><input type="hidden" id="font_total_record" value="'.$totalRecord.'" /> '
 	);
	
	$form['webfonts_selectors_field'] = array(
	'#type' => 'fieldset',
	'#title' => '<strong>Drupal Existing Selector</strong>',
	'#prefix'=>'<div id="selectors" style="display:none">',
	'#suffix'=> '</div>',
	);
	//drupal css selectos								
	$form['webfonts_selectors_field']['webfonts_drupal css'] = array(
	'#prefix'=>"<div>",
	'#value'=>$output1,
	'#suffix'=>"</div>");
									
									
	$form['#prefix']= '<div id="second">';
	$form['#suffix']= '</div></div>';
	
	$form['redirect'] = FALSE;
	
	return $form;

}

/**
*Ajax function for checking if the webfonts editors option has 
*activated in other projects or not
*/
function webfonts_editor_status_ajax(){
	$project_id = $_GET['project_id'];
	$query = "SELECT  count(wfs_configure_id) as cnt,project_name FROM `".TBLCONFIG."` WHERE `user_id` = '$project_id' and `wysiwyg_enabled` = '1' group by project_name";
	$result = db_fetch_object(db_query($query, $project_id));
 	if($result->cnt >0 )
	{
		$ret = array(
      'message' => t('Editor already enabled in project !name. This selection will overwrite the existing selection', array('!name' => $result->project_name)),
	  'status' => 1
    	);
	} else {
	$ret = array(
      'status' => 0
    	);
	}
return drupal_json($ret);
}

/**
*Project Details page 
* return the form for configure/selectors/domain
*/
function webfonts_page($projectid) {
drupal_add_js("$(document).ready(function() {showWFSlogo()});", 'inline');
return drupal_get_form('webfonts_tab',$projectid).drupal_get_form('webfonts_configure',$projectid).drupal_get_form('webfonts_domain',$projectid).drupal_get_form('webfonts_selectors',$projectid);
}

/*
*Fetch all the fonts given a project key
@project key: string
*/
function getPrjFonts($project_key){
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Fonts/?wfspstart=0&wfsplimit=".FONT_LIMIT."&wfspid=".$project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlMsg = $wfs_api->wfs_getInfo_post();
	//Creating xml DOM Instance	
	$xmlDomObj = new DOMDocument();
	$xmlDomObj->loadXML($xmlMsg);
	$fonts = $xmlDomObj->getElementsByTagName( "Font" );
	$webfonts=array();
	$TotalRecords = $xmlDomObj->getElementsByTagName( "TotalRecords" );
	$webfonts['TotalRecords']= $TotalRecords->item(0)->nodeValue;
	
	$PageLimit = $xmlDomObj->getElementsByTagName( "PageLimit" );
	$webfonts['PageLimit']= $PageLimit->item(0)->nodeValue;
	
	$PageStart = $xmlDomObj->getElementsByTagName( "PageStart" );
	$webfonts['PageStart']= $PageStart->item(0)->nodeValue;
	
	foreach($fonts as $font){
		$fontids = $font->getElementsByTagName("FontID");
		$webfonts['fontid'][]= $fontids->item(0)->nodeValue;
		
		$FontNames = $font->getElementsByTagName("FontName");
		$webfonts['FontName'][]= $FontNames->item(0)->nodeValue;
		
		$FontPreviewTextLongs = $font->getElementsByTagName("FontPreviewTextLong");
		$webfonts['FontPreviewTextLong'][]= $FontPreviewTextLongs->item(0)->nodeValue;
		
		$FontFondryNames = $font->getElementsByTagName("FontFondryName");
		$webfonts['FontFondryName'][]= $FontFondryNames->item(0)->nodeValue;
		
		$FontCSSNames = $font->getElementsByTagName("FontCSSName");
		$webfonts['FontCSSName'][]= $FontCSSNames->item(0)->nodeValue;
		
		$FontLanguages = $font->getElementsByTagName("FontLanguage");
		$webfonts['FontLanguage'][]= $FontLanguages->item(0)->nodeValue;
		
		$FontSizes = $font->getElementsByTagName("FontSize");
		$webfonts['FontSize'][]= $FontSizes->item(0)->nodeValue;
		
		$EnableSubsettings = $font->getElementsByTagName("EnableSubsetting");
		$webfonts['EnableSubsetting'][]= $EnableSubsettings->item(0)->nodeValue; 
	}
	return $webfonts;
}
/*
*Fetch all the fonts given a project key
@project key: string
*/
function font_list_pagination($project_key){
	$wfs_details=getUnPass();
	$pageStart = $_GET['pageStart'];
	//Fetching the XML data from WFS
	$apiurl = "xml/Fonts/?wfspstart=".$pageStart."&wfsplimit=".FONT_LIMIT."&wfspid=".$project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlMsg = $wfs_api->wfs_getInfo_post();
	//Creating XML Instance
	$xmlDomObj = new DOMDocument();
	$xmlDomObj->loadXML($xmlMsg);
	$fonts = $xmlDomObj->getElementsByTagName( "Font" );
	$webfonts=array();
	foreach($fonts as $font){
		$fontids = $font->getElementsByTagName("FontID");
		$webfonts['fontid'][]= $fontids->item(0)->nodeValue;
		
		$FontNames = $font->getElementsByTagName("FontName");
		$webfonts['FontName'][]= $FontNames->item(0)->nodeValue;
		
		$FontPreviewTextLongs = $font->getElementsByTagName("FontPreviewTextLong");
		$webfonts['FontPreviewTextLong'][]= $FontPreviewTextLongs->item(0)->nodeValue;
		
		$FontFondryNames = $font->getElementsByTagName("FontFondryName");
		$webfonts['FontFondryName'][]= $FontFondryNames->item(0)->nodeValue;
		
		$FontCSSNames = $font->getElementsByTagName("FontCSSName");
		$webfonts['FontCSSName'][]= $FontCSSNames->item(0)->nodeValue;
		
		$FontLanguages = $font->getElementsByTagName("FontLanguage");
		$webfonts['FontLanguage'][]= $FontLanguages->item(0)->nodeValue;
		
		$FontSizes = $font->getElementsByTagName("FontSize");
		$webfonts['FontSize'][]= $FontSizes->item(0)->nodeValue;
		
		$EnableSubsettings = $font->getElementsByTagName("EnableSubsetting");
		$webfonts['EnableSubsetting'][]= $EnableSubsettings->item(0)->nodeValue; 
	}
	
	if(count($webfonts["FontName"])>0){
		for($i=0;$i< count($webfonts["FontName"]);$i++){
		$rows[] = array(
		'name' => '<div class="font_sep odd ">
		<div class="font_img" style="font-family:\''.$webfonts["FontCSSName"][$i].'\' !important;font-size:20px;">'.$webfonts["FontPreviewTextLong"][$i].'</div>
		<div class="fontnames"><u>'.$webfonts["FontName"][$i].'</u> | <u>'.$webfonts["FontFondryName"][$i].'</u>
		| <u>'.$webfonts["FontLanguage"][$i].'</u>
		'.$webfonts["FontSize"][$i].'
		</div></div>',);
		}
	}
		$pageLimit =$_GET['pageLimit'];
		$totalRecord = $_GET['totalRecords'];
		$contentDiv = $_GET['contentDiv'];
		$paginationDiv = $_GET['paginationDiv'];
		if($totalRecord!="" && $pageLimit!="" && (count($webfonts["FontName"])>0)){
			$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,$contentDiv,$paginationDiv,"?q=admin/settings/webfonts/fontslists/".$project_key);
			$pagination = $wfs_pg->getPagination();
		}
		$output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
print drupal_json(array('status' => TRUE, 'data' => $output,'pagination'=>$pagination));
}

/**
*Configure tab action page
*/
function webfonts_configure_submit($form, &$form_state){
//overwriting the editor enable option in all the projects to status 0 except the current project
db_query("UPDATE `".TBLCONFIG."` SET wysiwyg_enabled = '0' where wfs_configure_id != '%d'",$form_state['values']['hidden']);
if($form_state['values']['custom'] == 2){
//updating the current project content
db_query("UPDATE `".TBLCONFIG."` SET project_pages = '%s',
									project_page_option = '%d',
									project_day = '%s',
									wysiwyg_enabled = '%d',
									editor_select = '%d'
									 where wfs_configure_id = '%d'",
$form_state['values']['webfonts_visibility_pages_show'],$form_state['values']['custom'],$form_state['values']['webfonts_display_day'],$form_state['values']['enable_editor'],$form_state['values']['editor_select'],$form_state['values']['hidden']);
//If the user is not a free user
if(!empty($form_state['values']['selection_option']) || $form_state['values']['selection_option']==0){
	db_query("UPDATE `".TBLCONFIG."` SET project_options = '%d' where wfs_configure_id = '%d'",$form_state['values']['selection_option'],$form_state['values']['hidden']);
}
}
else{

//updating the current project content
db_query("UPDATE `".TBLCONFIG."` SET project_pages = '%s',
									project_page_option = '%d',
									project_day = '%s',
									wysiwyg_enabled = '%d',
									editor_select = '%d'
									 where wfs_configure_id = '%d'",
$form_state['values']['webfonts_visibility_pages'],$form_state['values']['custom'],$form_state['values']['webfonts_display_day'],$form_state['values']['enable_editor'],$form_state['values']['editor_select'],$form_state['values']['hidden']);
//If the user is not a free user
	if(!empty($form_state['values']['selection_option']) || $form_state['values']['selection_option']==0){
		db_query("UPDATE `".TBLCONFIG."` SET project_options = '%d' where wfs_configure_id = '%d'",$form_state['values']['selection_option'],$form_state['values']['hidden']);
		}
		
	}

}

/*
** fetch the font list drop down in selectors tab
*/
function webfonts_font_list($projectId, $defaultFont="null",$count){ 

 $result = array();
 $data = webfonts_project_profile_load($projectId);

	$options ='<select id="edit-fonts-list@'.$count.'" class="edit-fonts-list" name="font_list[]">';
	$options.= '<option value="-1" >- - - - - Please select a font- - - - --</option>'; 
	// XML feed file/URL 
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Fonts/?wfspid=".$data->project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//Creating xml DOM Instance	
	//creating a DOM object
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	
	//fetching XML data  
	$fonts = $doc->getElementsByTagName( "Font" );
	foreach( $fonts as $font )
	{
	$FontNames = $font->getElementsByTagName("FontName");	 
	$FontName = $FontNames->item(0)->nodeValue; 
	
	$FontCSSNames = $font->getElementsByTagName("FontCSSName");	 
	$FontCSSName = $FontCSSNames->item(0)->nodeValue; 
	
	$FontIDs = $font->getElementsByTagName("FontID");	 
	$FontID = $FontIDs->item(0)->nodeValue; 
		
	$FontPreviewTextLongs = $font->getElementsByTagName("FontPreviewTextLong");	 
	$FontPreviewTextLong = $FontPreviewTextLongs->item(0)->nodeValue; 
	
	$selected =($defaultFont == $FontID)?"Selected":"";
	if($defaultFont == $FontID){
		$fontCssName=$FontCSSName;
		$fontPreviewTextLong = $FontPreviewTextLong;
	}
	$options.= '<option value="'.$FontCSSName.'@!'.$FontPreviewTextLong.'@!'.$FontID.'" '.$selected.' >'.$FontName.'</option>'; 
	
		 
	}
$options.= '</select>';	
array_push($result,$options);
array_push($result,$fontPreviewTextLong);
array_push($result,$FontName);
array_push($result,$fontCssName);

return $result;
}
/***************************** SELECTORS SECTION **************************************************/
/**
* Selectors tab form submit function
*/
function webfonts_selectors_submit($form, &$form_state){

$case = $form_state[values][op];
switch($case){
	case 'Save':
		
		$fontIdList = array();
		$selectorIdList = array();
		
		foreach($form['#post']['font_list'] as $key => $fontname)
			{
			$fontidarr = explode("@!",$fontname);
			if($fontidarr[2] != ''){
				array_push($fontIdList,$fontidarr[2]);
			}else{
				array_push($fontIdList,'-1');
			} 
				$cnt = $key + 1;
				$selctor_id = $form['#post']['Selector_'.$cnt];
				array_push($selectorIdList,$selctor_id);
			}
			$fontids = implode(",",$fontIdList);
			$selectorsids = implode(",",$selectorIdList);
			
			//get user details
		$wfs_details=getUnPass();
		//get project info
		$data = webfonts_project_profile_load($_SESSION['webfonts_project_id']);
		//Fetching the xml data from WFS
		$apiurl = "xml/Selectors/?wfspid=".$data->project_key;
		$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
		$xmlUrl = $wfs_api->saveSelector($fontids,$selectorsids);
		//Creating JSON Instance
		if(!empty($xmlUrl)){
		//creating a DOM object
		$doc = new DOMDocument();
		$doc->loadXML($xmlUrl);
		$messages = $doc->getElementsByTagName( "Message" );
		$message = $messages->item(0)->nodeValue;
		if($message == "Success")
			{
			drupal_goto("admin/settings/webfonts/projectdetails/".$_SESSION['webfonts_project_id']);	
			}
		else{
			drupal_set_message($message,'error');
			drupal_goto("admin/settings/webfonts/projectdetails/".$_SESSION['webfonts_project_id']);
			}
		}
		break;	
	default:
		break;
	}
	
}

/**
 *Display the delete confirm message for selector delete function
 */
function webfonts_selectors_delete(&$form_state, $delta, $selector, $projectid){
	drupal_add_js("$(document).ready(function() {showWFSlogo()});", 'inline');
	$data = webfonts_project_profile_load($projectid);
	$projectkey=$data->project_key;
	
	//The question to ask the user.
    $question = t('Are you sure you want to delete the selector %selector?', array('%selector' => $selector));
   	
	// The page to go to if the user denies the action.
    $path = 'admin/settings/webfonts/projectdetails/'.$projectid;

    // Additional text to display (defaults to "This action cannot be undone.").
    $description = t('This action cannot be undone.');

    //  A caption for the button which confirms the action.
    $yes = t('Delete');

    // A caption for the link which denies the action.
    $no = t('Cancel');

    // set delta value to use in submit function.
    $form['delta'] = array('#type' => 'value', '#value' => $delta);
	$form['projectkey'] = array('#type' => 'value', '#value' => $projectkey);
	$form['projectid'] = array('#type' => 'value', '#value' => $_SESSION['webfonts_project_id']);
	
	
    
    // set the redirect path value to use in submit function.
    $form['#redirect'] = $path;
    return confirm_form(
      $form,
      $question,
      $path,
      $description,
      $yes,
      $no
    );
}

/**
 * Deleting the selector if user confirms to delete
 */
function webfonts_selectors_delete_submit($form, &$form_state) {
	//get user details
	$wfs_details=getUnPass();
	$project_key = $form['projectkey']['#value'];
	$selector_id = $form_state[values][delta];
	//Fetching the xml data from WFS
	$apiurl = "xml/Selectors/?wfspid=".$project_key."&wfsselector_id=".urlencode($selector_id);
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->deleteSelector();
	//Creating xml Instance
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	$messages = $doc->getElementsByTagName( "Message" );
	$Message = $messages->item(0)->nodeValue;

	//fetching Json data
	if($Message=="Success"){
		drupal_set_message("Selectors successfully deleted");
		
	}else{
		drupal_set_message($Message,"error");
	}
	drupal_goto("admin/settings/webfonts/projectdetails/".$form['projectid']['#value']);
	die(); 
}




/*
* Ajax based selectors adding function
*/
function webfonts_selectors_list($projectid,$selectorsname){
	$row=array();
	$errMsg = "";
	$data = webfonts_project_profile_load($projectid);
	//get user details
	$wfs_details=getUnPass();
	$pageStart = 0;
	//Fetching the xml data from WFS
	$apiurl = "xml/Selectors/?wfspstart=0&wfsplimit=".SELECTOR_LIMIT."&wfspid=".$data->project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->addSelector($selectorsname);
	//creating a DOM object
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	$messages = $doc->getElementsByTagName( "Message" );
	$message = $messages->item(0)->nodeValue;
	if($message == "Success"){
	//fetching XML data
	$selectors = $doc->getElementsByTagName( "Selector" );
	$count = 1;
	$selectorKey="";
	foreach( $selectors as $selector )
	{
		$SelectorTags = $selector->getElementsByTagName("SelectorTag");	
		$SelectorTag = $SelectorTags->item(0)->nodeValue;
	
		$SelectorIDs = $selector->getElementsByTagName("SelectorID");	
		$SelectorID = $SelectorIDs->item(0)->nodeValue;
		 
		$SelectorFontIDs = $selector->getElementsByTagName("SelectorFontID");	
		$SelectorFontID = $SelectorFontIDs->item(0)->nodeValue;
		
		$fontsArr = webfonts_font_list($projectid,$SelectorFontID,$count);
		
		$rows[] = array(
				'tag' => '<strong>'.$SelectorTag.'</strong>',
				'fonts' => $fontsArr[0] ,
      			'sample' => '<span style="font-size:26px;font-family:'.$fontsArr[3].'" id="fontid_'.$count.'">'.$fontsArr[1].'</span>',
      			'action' => l(t('Remove'), "admin/settings/webfonts/selectorsdelete/".$SelectorID."/".$SelectorTag."/".$projectid),
      				);
		
		$selectorKey.= '<input id="edit-Selector-'.$count.'" name="Selector_'.$count.'" value="'.$SelectorID.'" type="hidden" >';
		$count++;
		}
	}
	
	 if(!empty($message)){
	$msgdisplay = $message;
	$errMsg = $message;
	}
	 else{
	$msgdisplay = "No selectors available.";
	}
	//if non of the projects has been addeds
	if(empty($rows)){
	 $rows[] = array(array('data' => '<div style="text-align:center">'.$msgdisplay.'</div>', 'colspan' => '4'));
	 }
	
	$contentDiv = $_GET['contentDiv'];
	$paginationDiv = $_GET['paginationDiv'];
	$totalrecords = $doc->getElementsByTagName( "TotalRecords" );
	$totalRecord =$totalrecords->item(0)->nodeValue;
	
	$pagestarts = $doc->getElementsByTagName( "PageStart" );
	$pageStart =$pagestarts->item(0)->nodeValue;
	
	$pagelimits = $doc->getElementsByTagName( "PageLimit" );
	$pageLimit =$pagelimits->item(0)->nodeValue;
		
	$output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
 	if($count != 1 && $pageLimit!="" && $totalRecord!=""){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,'selectorList','selector_pagination_div',"?q=admin/settings/webfonts/selectors_list/".$projectid);
		$Pagination =$wfs_pg->getPagination();
	}
 $output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
 print drupal_json(array('status' => TRUE, 'data' => $output.$selectorKey,'errMsg' => $errMsg,'pagination'=>$Pagination));
}
/*
* Ajax based selectors pagination function
*/
function webfonts_selectors_list_pagination($projectid){
	$row=array();
	$errMsg = "";
	$data = webfonts_project_profile_load($projectid);
	//get user details
	$wfs_details=getUnPass();
	$pageStart = (!empty($_GET['pageStart']))?$_GET['pageStart']:0;
	// Fetching the Xml data form WFS
	$apiurl = "xml/Selectors/?wfspstart=".$pageStart."&wfsplimit=".SELECTOR_LIMIT."&wfspid=".$data->project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlUrl = $wfs_api->wfs_getInfo_post();
	//Creating XML DOM Instance
	$doc = new DOMDocument();
	$doc->loadXML($xmlUrl);
	$messages = $doc->getElementsByTagName( "Message" );
	$message = $messages->item(0)->nodeValue;
	if($message == "Success"){
	//fetching XML data
	$selectors = $doc->getElementsByTagName( "Selector" );
	$count = 1;
	foreach( $selectors as $selector )
	{
		$SelectorTags = $selector->getElementsByTagName("SelectorTag");	
		$SelectorTag = $SelectorTags->item(0)->nodeValue;
	
		$SelectorIDs = $selector->getElementsByTagName("SelectorID");	
		$SelectorID = $SelectorIDs->item(0)->nodeValue;
		 
		$SelectorFontIDs = $selector->getElementsByTagName("SelectorFontID");	
		$SelectorFontID = $SelectorFontIDs->item(0)->nodeValue;
		
		$fontsArr = webfonts_font_list($projectid,$SelectorFontID,$count);
		$rows[] = array(
				'tag' => '<strong>'.$SelectorTag.'</strong>',
				'fonts' => $fontsArr[0] ,
      			'sample' => '<span style="font-size:26px;font-family:'.$fontsArr[3].'" id="fontid_'.$count.'">'.$fontsArr[1].'</span>',
      			'action' => l(t('Remove'), "admin/settings/webfonts/selectorsdelete/".$SelectorID."/".$SelectorTag."/".$projectid),
      			);
		$selectorKey.= '<input id="edit-Selector-'.$count.'" name="Selector_'.$count.'" value="'.$SelectorID.'" type="hidden" >';
		$count++;
		}
	}
	
	if(!empty($message)){
	$msgdisplay = $message;
	$errMsg = $message;
	}
	 else{
	$msgdisplay = "No selectors available.";
	}
	//if non of the projects has been addeds
	if(empty($rows)){
	 $rows[] = array(array('data' => '<div style="text-align:center">'.$msgdisplay.'</div>', 'colspan' => '4'));
	 }
			
	$pageLimit =$_GET['pageLimit'];
	$totalRecord = $_GET['totalRecords'];
	$contentDiv = $_GET['contentDiv'];
	$paginationDiv = $_GET['paginationDiv'];
	$output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
 	$output.=$selectorKey;
	if($totalRecord!="" && $pageLimit!="" && $count != 1){
 		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,'selectorList','selector_pagination_div',"?q=admin/settings/webfonts/selectors_list/".$projectid);
		$Pagination =$wfs_pg->getPagination();
		}
 print drupal_json(array('status' => TRUE, 'data' => $output,'errMsg' => $errMsg,'pagination'=>$Pagination));
}

/*
* this function adds a selector to wfs
*/
function webfonts_extSelect($wfs_prjId,$wfs_selector){
	$wfs_selector=urldecode($wfs_selector);
	webfonts_selectors_list($wfs_prjId,$wfs_selector);
}
/******************************DOMAIN SECTION*****************************************/
/*
domain adding ajax page
**/
function webfonts_domains_list($domain_name,$projectId){
	$domain_name=urldecode($domain_name);
	$data = webfonts_project_profile_load($projectId);
	$project_key=$data->project_key;
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Domains/?wfspstart=0&wfsplimit=".DOMAIN_LIMIT."&wfspid=".$project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlmsg = $wfs_api->addDomain($domain_name);
	//Creating Xml Instance
	$doc = new DOMDocument();
	$doc -> loadXML($xmlmsg);
	$Messages = $doc->getElementsByTagName( "Message" );
	$Message = $Messages->item(0)->nodeValue;

	if($Message=="Success"){
		$ProjectIDs = $doc->getElementsByTagName( "ProjectID" );
		$ProjectID = $ProjectIDs->item(0)->nodeValue;
		
		$domains = $doc->getElementsByTagName( "Domain" );
			foreach( $domains as $domain ){
				$count_domain = 1;
				$domainNames = $domain->getElementsByTagName("DomainName");
				$domainName = $domainNames->item(0)->nodeValue;
				
				$domainIDs = $domain->getElementsByTagName("DomainID");
				$domainID = $domainIDs->item(0)->nodeValue;
				
				$rows[] = array(
				'name' => '<a href="http://'.$domainName.'" target="_blank">'.check_plain($domainName).'</a>',
				'action' => l(t('Edit'), "admin/settings/webfonts/domainedit/".$domainID."/".$projectId."/".$domainName)." | ".l(t('Remove'), "admin/settings/webfonts/domaindelete/".$domainID."/".$projectId."/".$domainName),);
			}
			//if non of the projects has been addeds
			if(empty($rows)){
				$rows[] = array(array('data' => '<div style="text-align:center">'.t('No Domains available.').'</div>', 'colspan' => '4'));
			}
			$contentDiv = $_GET['contentDiv'];
			$paginationDiv = $_GET['paginationDiv'];
			$totalrecords = $doc->getElementsByTagName( "TotalRecords" );
			$totalRecord =$totalrecords->item(0)->nodeValue;
				
			$pagestarts = $doc->getElementsByTagName( "PageStart" );
			$pageStart =$pagestarts->item(0)->nodeValue;
				
			$pagelimits = $doc->getElementsByTagName( "PageLimit" );
			$pageLimit =$pagelimits->item(0)->nodeValue;
			if($totalRecord!="" && $pageLimit!="" && $count_domain == 1){
				$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,$contentDiv,$paginationDiv,"?q=admin/settings/webfonts/domains/".$projectId);
				$pagination = $wfs_pg->getPagination();
			}
			$output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
			print drupal_json(array('status' => TRUE, 'data' => $output,'pagination'=>$pagination));
		}else{
			$output=$Message;
			print drupal_json(array('status' => FALSE, 'data' => $output));
}
}
 
/*
domain adding ajax page
**/
function webfonts_domains_list_pagination($projectId){
	$domain_name=urldecode($domain_name);
	$data = webfonts_project_profile_load($projectId);
	$project_key=$data->project_key;
	$wfs_details=getUnPass();
	$startPage = (isset($_GET['pageStart']))?$_GET['pageStart']:0;
	// Fetching the XML data form WFS
	$apiurl = "xml/Domains/?wfspstart=".$startPage."&wfsplimit=".DOMAIN_LIMIT."&wfspid=".$project_key;
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlmsg = $wfs_api->wfs_getInfo_post();
	//creating a DOM object
	$doc = new DOMDocument();
	$doc -> loadXML($xmlmsg);
	$Messages = $doc->getElementsByTagName( "Message" );
	$Message = $Messages->item(0)->nodeValue;
	
	if($Message=="Success"){
	$ProjectIDs = $doc->getElementsByTagName( "ProjectID" );
	$ProjectID = $ProjectIDs->item(0)->nodeValue;
	
	$domains = $doc->getElementsByTagName( "Domain" );
		foreach( $domains as $domain ){
		$domainNames = $domain->getElementsByTagName("DomainName");
		$domainName = $domainNames->item(0)->nodeValue;
		
		$domainIDs = $domain->getElementsByTagName("DomainID");
		$domainID = $domainIDs->item(0)->nodeValue;
		
		$rows[] = array(
		'name' => '<a href="http://'.$domainName.'" target="_blank">'.check_plain($domainName).'</a>',
		'action' => l(t('Edit'), "admin/settings/webfonts/domainedit/".$domainID."/".$projectId."/".$domainName)." | ".l(t('Remove'), "admin/settings/webfonts/domaindelete/".$domainID."/".$projectId."/".$domainName));
		$count_domain =1;
		}
	//if non of the projects has been addeds
	if(empty($rows)){
		$rows[] = array(array('data' => '<div style="text-align:center">'.t('No Domains available.').'</div>', 'colspan' => '4'));
	}
		$pageStart = $_GET['pageStart'];
		$pageLimit =$_GET['pageLimit'];
		$totalRecord = $_GET['totalRecords'];
		$contentDiv = $_GET['contentDiv'];
		$paginationDiv = $_GET['paginationDiv'];
		if($totalRecord!="" && $pageLimit!="" && $count_domain ==1){
		$wfs_pg = new wfs_pagination($totalRecord,$pageStart,$pageLimit,$contentDiv,$paginationDiv,"?q=admin/settings/webfonts/domains/".$projectId);
		$pagination = $wfs_pg->getPagination();
		}
		$output = theme('table', $header, $rows, array('id' => 'webfonts_fonts'));
		print drupal_json(array('status' => TRUE, 'data' => $output,'pagination'=>$pagination));
	}else{
		$output=$Message;
		print drupal_json(array('status' => FALSE, 'data' => $output));
	}
}
/**
 *Display the delete confirm message for domain delete function
 */
function webfonts_domains_delete(&$form_state, $domain_id,$project_id,$domain_name)
{
	drupal_add_js("$(document).ready(function() {showWFSlogo()});", 'inline');
	$domain_name=urldecode($domain_name);
	$data = webfonts_project_profile_load($project_id);
	$project_key=$data->project_key;
	//The question to ask the user.
    $question = t('Are you sure you want to delete the domain %domain?', array('%domain' => $domain_name));
   	
	// The page to go to if the user denies the action.
    $path = 'admin/settings/webfonts/projectdetails/'.$project_id;

    // Additional text to display (defaults to "This action cannot be undone.").
    $description = t('This action cannot be undone.');

    //  A caption for the button which confirms the action.
    $yes = t('Delete');

    // A caption for the link which denies the action.
    $no = t('Cancel');

    // set project_id value to use in submit function.
    $form['domain_id'] = array('#type' => 'hidden', '#value' => $domain_id);
	$form['project_key'] = array('#type' => 'hidden', '#value' => $project_key);
	
	$form['actions']['submit']['#attributes']=array('class' => 'wfs_b_s','prefix' =>'<span class="wfs_b_o"><span class="wfs_b_i">','suffix' =>'</span></span>');
    
    // set the redirect path value to use in submit function.
    $form['#redirect'] = $path;
    return confirm_form(
      $form,
      $question,
      $path,
      $description,
      $yes,
      $no
    );
}

/**
 * Deleting the selector if user confirms to delete
 */
function webfonts_domains_delete_submit($form, &$form_state) {
	$domain_id=$form_state['values']['domain_id'];
	$project_key=$form_state['values']['project_key'];
	$wfs_details=getUnPass();
	//Fetching the xml data from WFS
	$apiurl = "xml/Domains/?wfspid=".$project_key."&wfsdomain_id=".urlencode($domain_id);
	$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
	$xmlMsg = $wfs_api->deleteDomain();
	//Creating XML Instance
	$doc = new DOMDocument();
	$doc->loadXML($xmlMsg);
	$Messages = $doc->getElementsByTagName("Message");	
	$Message = $Messages->item(0)->nodeValue;
	
	//fetching XML data
	if($Message=="Success"){
		$domains = $doc->getElementsByTagName( "Domains" );
		foreach($domains as $domain){
			$ProjectIDs = $domain->getElementsByTagName("ProjectID");	
			$ProjectID = $ProjectIDs->item(0)->nodeValue;
		}
		drupal_set_message("Domain successfully deleted","status");
	}else{
		drupal_set_message($Message,"warning");
	}
}

/**
 *Display the delete confirm message for domain delete function
 */
function webfonts_domains_edit(&$form_state, $domain_id, $project_id, $domain_name){
	drupal_add_js("$(document).ready(function() {showWFSlogo()});", 'inline');
	$domain_name=urldecode($domain_name);
	drupal_add_css(drupal_get_path('module', 'webfonts') .'/webfonts.css');
	// create a fieldset for domain edit
	$form=array();
	$form['domain_list'] = array(
	'#type' => 'fieldset',
	'#title' => t('Edit domain for <strong>'.$domain_name.'</strong> project'),
	);
  
	//edit domain_name
	$form['domain_list']['domain_name'] = array(
	'#type' => 'textfield',
	'#title' => t('Domain Name'),
	'#description' => "change the domain name details",
	'#default_value' => $domain_name,
	);
	
	//edit domain_id
	$form['domain_list']['domain_id'] = array(
	'#type' => 'hidden',
	'#default_value' => $domain_id,
	);
	
	//edit project_key
	$form['domain_list']['project_id'] = array(
	'#type' => 'hidden',
	'#default_value' => $project_id,
	);
  
	//Add and import projects button
	$form['domain_list']['save'] = array(
	'#type' => 'submit',
	'#prefix' =>'<span class="wfs_b_o"><span class="wfs_b_i">',
	'#suffix' =>'</span></span>',
	'#value' => t('Save'),
	);
	
	$form['domain_list']['save']['#attributes']=array('class' => 'wfs_b_s');
	
	$form['domain_list']['cancel'] = array(
	'#type' => 'markup',
	'#validate' => '', // This will cause this button to skip all validation. Put a function name here for validation with that function.
	'#value' => l(t('Cancel'), referer_uri()),
	'#submit' => 'my_custom_cancel',
	);

	return $form;
}

/*
* submit page of domain edit
*/
function webfonts_domains_edit_submit($form, &$form_state){
	if($form_state['values'][form_id]=="webfonts_domains_edit"){
		$domain_name=$form_state['values'][domain_name];
		$domain_id=$form_state['values'][domain_id];
		$project_id=$form_state['values'][project_id];
		$data = webfonts_project_profile_load($project_id);
		$project_key=$data->project_key;
		$wfs_details=getUnPass();
	
		//Fetching the XML data from WFS
		$apiurl = "xml/Domains/?wfspid=".$project_key.'&wfsdomain_id='.urlencode($domain_id);
		$wfs_api = new Services_WFS($wfs_details[1],$wfs_details[2],$apiurl);
		$xmlMsg = $wfs_api->editDomain($domain_name);
		
		//Creating XML Instance
		$doc = new DOMDocument();
		$doc->loadXML($xmlMsg);
		$Messages = $doc->getElementsByTagName("Message");	
		$Message = $Messages->item(0)->nodeValue;
		
		//fetching XML data
		if($Message=="Success"){
			drupal_set_message("Domain successfully updated","status");
		}else{
			drupal_set_message($Message,"warning");
		}
		drupal_goto("admin/settings/webfonts/projectdetails/".$project_id);
		die();
	}
}

