/*webfonts.js*/
/****
Copyright 2010 Monotype Imaging Inc.  
This program is distributed under the terms of the GNU General Public License
****/

var pathname=window.location.pathname;
var newstring=pathname.split("/");
var Folder_name ="webfontsdrupalxmlwithouteditor";
var fullpath= window.location.protocol+"//"+window.location.host+"/"+newstring[1];

var editor;
// JavaScript Document
$(document).ready(function() {
	image1 = new Image();
	image1.src = fullpath+"/modules/"+Folder_name+"/images/loading.gif";
	image2 = new Image();
	image2.src = fullpath+"/modules/"+Folder_name+"/images/ajax-loader.gif";
	
	 $('.addtowebfonts').click(function() {
		string=this.id
		wfstest=string.split(":");
		var oImg=document.createElement("img");
		oImg.src=fullpath+"/modules/"+Folder_name+"/images/loading.gif";
		var old_value =  $('#selectorList').html();
		document.getElementById('div'+wfstest[0]).appendChild(oImg);
		
		$.ajax({
   			type: "GET",
			url: "?q=admin/settings/webfonts/extSelect/"+wfstest[1]+"/"+wfstest[0],
   			data: "",
			dataType: "json",
   			success: function(msg){
				if(msg){
				document.getElementById('selectorList').innerHTML=msg.data;
				document.getElementById('div'+wfstest[0]).innerHTML="Added to webfonts";
				initBinding()
				}
   			},
			error: function (xhr, ajaxOptions, thrownError){
				alert(xhr.status);
				alert(thrownError);
            }
 		});
	 });
	 
	 $('#edit-webfonts-display-day').blur(function(e){
			var wfsdd = $("input[name=webfonts_display_day]").val();
			wfsdd = wfsdd.replace(/\d/g, "");
			wfsdd = wfsdd.replace(/-/g, "");
			wfsdd = wfsdd.replace(/,/g, "");
			wfsdd = wfsdd.replace(/ /g, "");
			if (wfsdd!='') {
				alert("Invalid display day");
				setTimeout(function(){$("#edit-webfonts-display-day").focus();$("#edit-webfonts-display-day").select();}, 10);
				
		}
		return false;
	 });
	 
	 /*save the edited*/
	 $('#edit-save').click(function() {
		 if($("input[name=domain_name]").val()==''){
			 alert('Please enter a domain name.');
			 return false;
		 }
	 });
	 
	 $('#edit-submit-domain').click(function() {
		 adddomain();
		 return false;
	 });
	 
	$('#edit-add-domain').keypress(function(e){
	if(e.keyCode == 13){
		adddomain();
		return false;
	}
	});
	
	
	 
	$('#edit-refresh').click(function() {
	var old_value =  $('#checkboxes').html();
	$('#checkboxes').html('<img src="'+fullpath+'/modules/'+Folder_name+'/images/ajax-loader.gif" width="220" height="19" />');
	var pageLimit = $("#prj_page_limit").val();
	var pageStart = $("#prj_page_start").val();;
	var totalRecords = $("#prj_total_record").val();;
	$.ajax({
   			url: "?q=admin/settings/webfonts/projects",
			dataType: "json",
   			data: {pageLimit:pageLimit,pageStart:pageStart,totalRecords:totalRecords,currentpage:0,contentDiv:"checkboxes",paginationDiv:"project_pagination_div"},
			success: function(msg){
				if(msg){
								
					if(msg.dataNo > 0){
					$('#checkboxes').hide().html(msg.data).fadeOut('slow').show();
					document.getElementById("project_pagination_div").innerHTML=msg.pagination;
					$('#addbutton').css('display','block');
					}
					else{
					document.getElementById('checkboxes').innerHTML=old_value;
						}
					
				}else{
					document.getElementById('checkboxes').innerHTML=old_value;
				}
   			},
			error: function (xhr, ajaxOptions, thrownError){
				alert(xhr.status);
				alert(thrownError);
            }
 		});
		return false;
	 });
	 
	 
	 $('#edit-submit-selector').click(function() {
		 addSelectores()
		 return false;
				   });
	 $('#edit-add-selector').keypress(function(e) {
		if(e.keyCode == 13)
		{
			addSelectores()
			return false;
			}
		});
	 
	 
  		$("#editorShow").click(function(){
										
			var anchor_val 
			var showText = $("#editorShow").text();
			
			if(showText == "See online editor with web fonts")
				{$('#editorshowload').html('<img src="'+fullpath+'/modules/'+Folder_name+'/images/loading.gif"  />');				
				anchor_val = "Hide online editor";
				if ( editor ){
				return;
				}
				var html = document.getElementById( "contents" ).innerHTML;
				var config = {};
				editor = CKEDITOR.appendTo( "webfontsEditor", config, html );
				$("#webfontsEditor").css('display','block');
				$("#contents").css('display','block');
				
				}
			else 
				{
				if ( !editor )
				return;
				$("#webfontsEditor").css('display','none');
				$("#contents").css('display','none');
				anchor_val = "See online editor with web fonts";
				document.getElementById( "contents" ).innerHTML = editor.getData();
			
				// Destroy the editor.
				editor.destroy();
				editor = null;
				
				}
			setTimeout("$('#editorshowload').html('')", 5000);
			 
			$("#editorShow").text(anchor_val);
		});
	 
	 $('#edit-preview-font').click(function(){
					
				
	//var content = CKEDITOR.instances.txtEdit.getData();
		var content =editor.getData();
//	alert(content);
	content1 = content.replace(/<\/?[^>]+>/g, "");
	var filtered_content = extractUniqueChars(content1);
	//alert(filtered_content);
	var projectId = $('#edit-cur-project-key').val();
	var projectFontList = WFS_DOM_fontFamilies[projectId];
	var currentCSSText = "";

	 var html = "";
	WFS.foreach(projectFontList, function (i, val) {
							var fontFamily = val["font-family"];
								var url = "";
							var fontType = "TTF";
               				 if (WFS.isIE) {
                    			fontType = "EOT";
                				}
									if(val["contentId"] !=null)
							{
								var fcid = val["contentId"][fontType];	
							}
							if (val["ApplyDynamicSubsetting"] === true) {
                    					url += WFS.dynamicFontContentURL.replace("{contentId}", fcid);
										url = url.replace("fctypeId=",escape(new Date()) + "&fctypeId=");
                    					url = url.replace("{content}", escape(filtered_content + fontFamily + "gi")).replace("{contentTypeId}", (WFS.isIE)? "2" : "1");
                					}
								else {
                    					url += WFS.staticFontContentURL.replace("{contentId}", fcid).replace("{type}", fontType.toLowerCase());
                					}
									
									url = url.replace("{cdnKey}", WFS_DOM_cdnkey).replace("{projectId}", projectId);
if (val["ApplyDynamicSubsetting"] === true) {
									currentCSSText += "@font-face { font-family:'" + fontFamily + "';  src : url('" + url + "'); }";
}
							  html += "<span style=\"font-family:'" + fontFamily + "';\" >"+content1+"</span>";
               
							
							});
	
	editor.destroy();
	CKEDITOR.on( 'instanceCreated', function( e ){
	e.editor.addCss( currentCSSText);
});
	
	//var html = document.getElementById( "contents" ).innerHTML;
	var config = {};
	editor = CKEDITOR.appendTo( "webfontsEditor", config, content );
	return false;
			});
	 
	 $("#edit-webfonts-token-checkbox").click(function(){
		
			$("#token_div").toggle();
		
													   });
	 
});
 function adddomain(){
		 if($("input[name=add_domain]").val()==''){
			 alert('Please enter a domain name.');
			 return false;
		 }
		var prjkey = $("input[name=project_key]").val();
		var domain_name = $("input[name=add_domain]").val();
		domain_name = domain_name.replace(/^http:\/\//ig, "");
		domain_name = $.URLEncode(domain_name.replace(/\/.*/ig, ""));
		var pageLimit = $("#domain_page_limit").val();
		var pageStart = $("#domain_page_start").val();;
		var totalRecords = $("#domain_total_record").val();;
		var old_value =  $('#domainList').html();
		$('#domainList').html('<img src="'+fullpath+'/modules/'+Folder_name+'/images/ajax-loader.gif" width="220" height="19" />');
		$.ajax({
   			type: "GET",
			url: "?q=admin/settings/webfonts/adddomains/"+domain_name+"/"+prjkey,
			dataType: "json",
			data: {pageLimit:pageLimit,pageStart:pageStart,totalRecords:totalRecords,currentpage:0,contentDiv:"domainList",paginationDiv:"domain_pagination_div",url:"?q=admin/settings/webfonts/domains/"+prjkey},
   			success: function(msg){
				if(msg.status){
					$('#domainList').hide().html(msg.data).fadeOut('slow').show();
					if(msg.pagination){
					document.getElementById("domain_pagination_div").innerHTML=msg.pagination;
					}
					initBinding();					
				}else{
					$('html, body').animate({scrollTop:0}, 'fast');	
					$('#errMsg').hide().html(msg.data).slideDown(700).show();
					setTimeout("$('#errMsg').slideUp(1000)", 5000);
					$('#domainList').html(old_value);
					//alert(msg.data);
				}
   			},
			error: function (xhr, ajaxOptions, thrownError){
				alert(xhr.status);
				alert(thrownError);
            }
 		});
	 }
function addSelectores(){
		if($('#edit-add-selector').val() == "")
			{
				alert('Please enter the selectors name.');
				return false;
			}
		var old_value =  $('#selectorList').html();	
		var pageLimit = $("#selector_page_limit").val();
		var pageStart = $("#selector_page_start").val();;
		var totalRecords = $("#selector_total_record").val();;
	  $('#selectorList').html('<img src="'+fullpath+'/modules/'+Folder_name+'/images/ajax-loader.gif" width="220" height="19" />');
		var pid = $('#edit-projectid').val();
		var selectorsname = $('#edit-add-selector').val().replace("#","%23");
		$.ajax({
   			type: "GET",
			url:"?q=admin/settings/webfonts/addselectors/"+pid+"/"+selectorsname,
   			dataType: "json",
			data: {pageLimit:pageLimit,pageStart:pageStart,totalRecords:totalRecords,currentpage:0,contentDiv:"selectorList",paginationDiv:"selector_pagination_div",url:"?q=admin/settings/webfonts/selectors_list/"+pid},
   			success: function(msg){
				
				if(msg){
					if(msg.errMsg == "DuplicateSelectorName")
					{
					$('html, body').animate({scrollTop:0}, 'fast');	
					$('#errMsg').hide().html('Selector name already exists').slideDown(700).show();
					setTimeout("$('#errMsg').slideUp(1000)", 5000);
					$('#selectorList').html(old_value);
					return false;
					}else{
				$('#selectorList').hide().html(msg.data).fadeOut('slow').show();
				
				if(msg.pagination){
					document.getElementById("selector_pagination_div").innerHTML=msg.pagination;
					}
				}		   
				initBinding();
				}
   			},
			error: function (xhr, ajaxOptions, thrownError){
				alert(xhr.status);
				alert(thrownError);
            }
 		});
	 
	  return false;
	  
	}

function initBinding(){
	
		$(".edit-fonts-list").change(function(){
				var fontcssid = this.id;
				var fontid = fontcssid.split("@");
				var fontarr = this.value;
				var fontdata = fontarr.split("@!");
				if(fontdata == -1)
				{
				$("#fontid_"+fontid[1]).text('');
				}
				else{
				$("#fontid_"+fontid[1]).css("font-family","'"+fontdata[0]+"'");
				$("#fontid_"+fontid[1]).text(fontdata[1]);
				}
			});	
		}
		
$.extend({URLEncode:function(c){var o='';var x=0;c=c.toString();var r=/(^[a-zA-Z0-9_.]*)/;
  while(x<c.length){var m=r.exec(c.substr(x));
    if(m!=null && m.length>1 && m[1]!=''){o+=m[1];x+=m[1].length;
    }else{if(c[x]==' ')o+='+';else{var d=c.charCodeAt(x);var h=d.toString(16);
    o+='%'+(h.length<2?'0':'')+h.toUpperCase();}x++;}}return o;},
URLDecode:function(s){var o=s;var binVal,t;var r=/(%[^%]{2})/;
  while((m=r.exec(o))!=null && m.length>1 && m[1]!=''){b=parseInt(m[1].substr(1),16);
  t=String.fromCharCode(b);o=o.replace(m[1],t);}return o;}
});

/*function addStyle(){

	var cnt = 0;
	var oldOnclick =$("a[title='Font Name']").attr('onclick');
	$("a[title='Font Name']").attr('onclick',null);
	$("a[title='Font Name']").click(function(e){
	
		oldOnclick.call(this,e);
		setTimeout(function(){
		var iFrame =$("iframe[title='Font Name']");
		var doc = iFrame.attr("contentDocument") || iFrame.attr("contentWindow").document;
		if(cnt == 0 ){
			var linkNode = doc.createElement("LINK");
			linkNode.type = "text/css";
			linkNode.href = fullpath+"/admin/settings/webfonts/font";
			linkNode.rel = "stylesheet";
			var head = doc.getElementsByTagName("HEAD")[0];
			$(head).append(linkNode);
			cnt++;
			}
		}, 200);
	})
	
	}*/
	
	function addStyle(){
	
	var cnt = 0;
	var oldOnclick =$("a[title='Font Name']").attr('onclick');
	$("a[title='Font Name']").attr('onclick',null);
	$("a[title='Font Name']").click(function(e){
		
		
		oldOnclick.call(this,e);
		setTimeout(function(){
		var iFrame =$("iframe[title='Font Name']");
		var doc = iFrame.attr("contentDocument") || iFrame.attr("contentWindow").document;
		if(cnt == 0 ){
			
			var linkNode = doc.createElement("LINK");
			linkNode.type = "text/css";
			linkNode.href = fullpath+"/admin/settings/webfonts/font";
			linkNode.rel = "stylesheet";
			linkNode.id = "mti-dropdown-css";
			var head = doc.getElementsByTagName("HEAD")[0];
			$(head).append(linkNode);
			cnt++;
			}else{
				if(IsIE8Browser() == true){
				var href = $("#mti-dropdown-css", doc).attr("href");
					$("#mti-dropdown-css", doc).attr("href", href);
				}
			}
			
		
		var iFrameText =$("iframe[title ^='Rich text editor']");
		var docText = iFrameText.attr("contentDocument") || iFrameText.attr("contentWindow").document;
		
		$(".cke_panel_list li a", doc).unbind("click.mti").bind("click.mti",function(){
	 if(IsIE8Browser() == true){	setTimeout(function(){
			$("link[href=?q=admin/settings/webfonts/font]", docText).attr("href", "?q=admin/settings/webfonts/font");
								}, 1000);}
			});
		
			
		}, 200);
	})
	}
	
function IsIE8Browser() {

    var rv = -1;

    var ua = navigator.userAgent;

    var re = new RegExp("Trident\/([0-9]{1,}[\.0-9]{0,})");

    if (re.exec(ua) != null) {

        rv = parseFloat(RegExp.$1);

    }
return (rv == 4);

}

//putting webfonts logo at top
function showWFSlogo(){
	$('<img src=\''+fullpath+'/modules/'+Folder_name+'/images/webfonts_fonts.gif\' alt=\'Fonts.com Web Fonts\' title=\'Fonts.com Web Fonts\' />').insertAfter('.breadcrumb');
}
//end putting webfonts logo at top

//Ajax based pagination code//
function ajaxPage(currPage,startPage,pageLimit,totalrecords,contentDiv,paginationDiv,url){
	var old_value =  $('#'+contentDiv).html();							
	$('#'+contentDiv).html('<img src="'+fullpath+'/modules/'+Folder_name+'/images/ajax-loader.gif" width="220" height="19" />');
	
	$.ajax({
		   type: "GET",
   			/*url: "?q=admin/settings/webfonts/projects",*/
			url: url,
			dataType: "json",
			data: {pageLimit:pageLimit,pageStart:startPage,totalRecords:totalrecords,currentpage:currPage,contentDiv:contentDiv,paginationDiv:paginationDiv,url:url},
			success: function(msg){
			
				if(msg){
					document.getElementById(contentDiv).innerHTML=msg.data;
					if(msg.pagination){
					document.getElementById(paginationDiv).innerHTML=msg.pagination;
					}
					if(contentDiv == "selectorList"){
						initBinding();
						}
				}else{
					document.getElementById(contentDiv).innerHTML=old_value;
					}
			},
			error: function (xhr, ajaxOptions, thrownError){
				alert(xhr.status);
				alert(thrownError);
            }
 		});
	

	
	
	
}

//end of Ajax based pagination//