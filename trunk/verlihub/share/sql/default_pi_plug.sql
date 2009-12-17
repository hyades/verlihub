INSERT IGNORE INTO pi_plug (nick,autoload,path,detail) VALUES
	("isp"    ,0,"/usr/local/lib/libisp_pi.so"   ,"Internet Service Provider settings, country codes, nick prefix, desc tag,..."),
	("lua"    ,0,"/usr/local/lib/liblua_pi.so"   ,"Support for lua scripts"),
	("perl"   ,0,"/usr/local/lib/libperl_pi.so"  ,"Support for perl scripts"),
	("msg"    ,0,"/usr/local/lib/libmessanger_pi.so","Offline messages system"),
	("flood"  ,0,"/usr/local/lib/libfloodprot_pi.so","Advanced flood protection"),
	("log"    ,0,"/usr/local/lib/libiplog_pi.so","Log ip's, nicks; history commands"),
	("forbid" ,0,"/usr/local/lib/libforbid_pi.so","Filter chat from forbidden words"),
	("chat"   ,0,"/usr/local/lib/libchatroom_pi.so","Multiple chatrooms to separate chat topics"),
	("replace",0,"/usr/local/lib/libreplace_pi.so","Replace some words by other"),
	("stats"  ,0,"/usr/local/lib/libstats_pi.so","Statistics plugin, trace diverse value sin the database")
