require("luci.sys")
require("luci.sys.zoneinfo")  
require("luci.tools.webadmin")  
require("nixio.fs")  
require("luci.config") 
require("luci.model.uci")
local m, s, o  
  
m = Map("/opt/ft/ftconfig", translate("Device Config"), translate("Device settings"))
m:chain("luci") 
  
s = m:section(TypedSection, "ftconfig", translate("Device Config"))
s.addremove = false
s.anonymous = true

x_sn = s:option(Value, "sn", translate("SN"))
function x_sn.cfgvalue(...)
	local retval = luci.sys.exec("uci -c/opt/ft get ftconfig.@ftconfig[0].sn")
	retval = string.sub(retval, 1, -2)
	return retval	
end

x_passwd = s:option(Value, "master_passwd", translate("Master_Password"))
x_passwd.password = true
--x_passwd.rmempty = false
function x_passwd.cfgvalue(...)
	local retval = luci.sys.exec("uci -c/opt/ft get ftconfig.@ftconfig[0].master_passwd")
	retval = string.sub(retval, 1, -2)
	return retval
end

x_musicpasswd = s:option(Value, "Music_passwd", translate("Music_Password"))
x_musicpasswd.password = true
function x_musicpasswd.cfgvalue(...)
	local retval = luci.sys.exec("uci -c/opt/ft get ftconfig.@ftconfig[0].musicpwd")
	retval = string.sub(retval, 1, -2)
	return retval
end

x_addr = s:option(Value, "master_addr", translate("Master_Address"))
--x_addr.datatype = "ipaddr"
--x_addr.rmempty = false
function x_addr.cfgvalue(...)
	local retval = luci.sys.exec("uci -c/opt/ft get ftconfig.@ftconfig[0].master_addr")
	retval = string.sub(retval, 1, -2)
	return retval
end

x_port = s:option(Value, "master_port", translate("Master_Port"))
function x_port.cfgvalue(...)
	local retval = luci.sys.exec("uci -c/opt/ft get ftconfig.@ftconfig[0].master_port")
	retval = string.sub(retval, 1, -2)
	return retval
end

x_musicmonth = s:option(Value, "musicmonth", translate("Music_Month"))
function x_musicmonth.cfgvalue(...)
	local retval = luci.sys.exec("uci -c/opt/ft get ftconfig.@ftconfig[0].musicmonth")
	retval = string.sub(retval, 1, -2)
	return retval
end

function s.cfgsections()
	return { "_xmpp" }
end


function m.on_commit(map)
	
	local masteraddr = x_addr:formvalue("_xmpp")
	local masterport = x_port:formvalue("_xmpp")
	local masterpasswd = x_passwd:formvalue("_xmpp")
	local musicpasswd = x_musicpasswd:formvalue("_xmpp")
	local musicmonth = x_musicmonth:formvalue("_xmpp")
	local sn = x_sn:formvalue("_xmpp")

        luci.sys.exec("cp /opt/ft/ftconfig /opt/ft/ftconfig1")
	luci.sys.exec("uci -c/opt/ft set ftconfig1.@ftconfig[0].master_addr="..masteraddr)
	luci.sys.exec("uci -c/opt/ft set ftconfig1.@ftconfig[0].master_port="..masterport)
	luci.sys.exec("uci -c/opt/ft set ftconfig1.@ftconfig[0].master_passwd="..masterpasswd)
	luci.sys.exec("uci -c/opt/ft set ftconfig1.@ftconfig[0].sn="..sn)
	luci.sys.exec("uci -c/opt/ft set ftconfig1.@ftconfig[0].musicpwd="..musicpasswd)
	luci.sys.exec("uci -c/opt/ft set ftconfig1.@ftconfig[0].musicmonth="..musicmonth)
  luci.sys.exec("uci -c/opt/ft commit ftconfig1")
  luci.sys.exec("cp /opt/ft/ftconfig1 /opt/ft/ftconfig")
	luci.sys.exec("rm /opt/ft/ftconfig1")
	m.message = translate("Device config success")

end

return m
