package Analyzer;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.ServerSocket;
import java.util.logging.Level;

import Config.Config;
import Config.Shutdown;

//log4j-*.*.**.jar for these imports
import org.apache.log4j.Logger;
import org.apache.log4j.BasicConfigurator;

public class Analyzer
{
	private static Logger _log = Logger.getLogger(Analyzer.class.getName());

	private static final String LOG_PROP = "./config/log.ini";
	
	private static Thread _listener1;
	private static Listen _list1;
	
	private static Thread _listener2;
	private static ListenUpdate _list2;
	
//	private static Tcpdump _dumper;

	public static void main(final String[] args) throws Exception 
	{
		BasicConfigurator.configure();	//configure the log4j logger
		
		File logFolder = new File("log");
		logFolder.mkdir();

//		try {
//			InputStream is = new BufferedInputStream(new FileInputStream(
//					LOG_PROP));
//			LogManager.getLogManager().readConfiguration(is);
//			is.close();
//		} catch (IOException e) {
//			_log.log(Level.SEVERE, "Failed to Load " + LOG_PROP + " File.", e);
//			System.exit(0);
//		}
		
		_log.info("The server has started");
		try 
		{
			Config.g().load();	//creates an instance of the config class, which is globally accessible
		}
		catch (Exception e) 
		{
			_log.fatal(e.getLocalizedMessage());
			System.exit(0);
		}

		_log.info("Server configuration loaded");
		
		//add a hook for when the server is shutdown
		Runtime.getRuntime().addShutdownHook(Shutdown.getInstance());
		
//		_dumper = new Tcpdump();
//		try
//		{
//			_dumper.init();
//		}
//		catch (Exception e) 
//		{
//			_log.fatal(e.getLocalizedMessage());
//			System.exit(0);
//		}

		//start listening for game server connections
		ServerSocket _tempSock;
		_tempSock = new ServerSocket(Config.PORT_NUM);
		_list1 = new Listen(_tempSock);
		_listener1 = new Thread(_list1);
		_listener1.start();
		
		//start listening for update server connections
		_tempSock = new ServerSocket(2003);
		_list2 = new ListenUpdate(_tempSock);
		_listener2 = new Thread(_list2);
		_listener2.start();
	}
}
