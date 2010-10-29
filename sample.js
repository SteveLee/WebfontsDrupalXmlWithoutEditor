/****
Copyright 2010 Monotype Imaging Inc.  
This program is distributed under the terms of the GNU General Public License
****/

var WFS = 
{
	isIE : (navigator.appName.toLowerCase().indexOf("microsoft internet explorer") >= 0), //Check for IE
	hideBody:function(){//seems quite clear
		document.body.style.visibility = "hidden";
	},
	showBody:function(){
		document.body.style.visibility = "visible";
	},
	extractUniqueChars: function (str) {
            if (str && typeof (str) == "string") {
                str = str.replace(/\s/g, "").replace(/\n/g,"").replace(/\r/g,"");
                var uniq = "", textLength = str.length, curChar = null;
                for (var i = 0; i < textLength; i++) {
                    curChar = str.charAt(i);
                    if (uniq.indexOf(curChar) == -1) {
                        uniq += curChar;
                    }
                }
                return uniq;
            }
            return "";
	},
	foreach: function (array, callback) {
            if (array.constructor == Array && callback.constructor == Function) {
                var i = 0;
                for (i = 0; i < array.length; i++) {
                    callback(i, array[i]);
                }
                return;
            }
            else if (array.constructor == Object && callback.constructor == Function) {
                for (prop in array) {
                    callback(prop, array[prop]);
                }
                return;
            }
    },
	indexOf: function(array, value){
			if(Array.indexOf){
				return array.indexOf(value);
			}
			else{
				for(var i=0; i<this.length; i++){
    				if(this[i]==obj){
     					return i;
    				}
   				}
   				return -1;
  			}
	},
	liveAPI : "http://api.fonts.com/FontFaceCssHandler.axd?projectid=",
	dynamicFontContentURL: "http://api.fonts.com/fontcontenthandler.axd?fctypeId={contentTypeId}&fcId={contentId}&projectId={projectId}&content={content}",
	staticFontContentURL: "http://fast.fonts.com/d/{contentId}.{type}?{cdnKey}&projectId={projectId}"
	//fontContentURL: "http://webfonts.fonts.com/FontContent_{domain}_{contentId}.axd?ProjectId={projectId}&content={content}" //content url with appropriate place holders
};	
	

function extractUniqueChars(str){
	
            if (str && typeof (str) == "string") {
                str = str.replace(/\s/g, "").replace(/\n/g,"").replace(/\r/g,"");
                var uniq = "", textLength = str.length, curChar = null;
                for (var i = 0; i < textLength; i++) {
                    curChar = str.charAt(i);
                    if (uniq.indexOf(curChar) == -1) {
                        uniq += curChar;
                    }
                }
                return uniq;
            }
            return "";
	
	}
	

 

