#!/usr/bin/env pythong
#
# plugin to load file coming from pinlog tool
#                       deroko of ARTeam

import	idaapi
import  idc
import	struct
import  colorsys

class loadlog_plugin_t(idaapi.plugin_t):
        flags = 0
        comment     = "Load logs produced by pinlog"
        help        = "Press Ctrl+h and select log file to load"
        wanted_name = "pinlog loadlog"
        wanted_hotkey = "Ctrl+h"    
        
        def init(self):
                self.clist = self.build_color_list();
                return idaapi.PLUGIN_KEEP;
            
        def build_color_list(self):
                clist = [];
                
                r = 0x00/255.0;
                g = 0x00/255.0;
                b = 0xff/255.0;
                
                (h,s,v) = colorsys.rgb_to_hsv(r,g,b);
                s -= 0.35;
                for x in range(0,15):
                        (r,g,b) = colorsys.hsv_to_rgb(h,s,v);
                        #it's BGR
                        ida_color = (int(b*255) << 16) + (int(g*255)<<8) + int(r*255);
                        clist.append(ida_color); 
                        s -= 0.04;
                clist.reverse();       
                return clist;
        
        def run(self, arg):                
        	filepath = idaapi.askfile_c(False, "*.*", "Pin log file");
        	imagebase = idaapi.get_imagebase();
        	try:
        		f = open(filepath, "rb");
        	except:
        		print("Need log file to parse data...");
        		return;
        	buff = f.read();
        	for index in range(0, len(buff)):
        		exec_count = ord(buff[index]);
        		if exec_count == 0:
        			continue;
        		
        		exec_count = exec_count / 10;
        		if exec_count >= len(self.clist): exec_count = len(self.clist)-1;
        		        
        		ida_color = self.clist[exec_count];
        
        		idc.SetColor(imagebase + index, CIC_ITEM, ida_color);
        
        def term(self):
                pass;
                    
def PLUGIN_ENTRY():
    return loadlog_plugin_t()
