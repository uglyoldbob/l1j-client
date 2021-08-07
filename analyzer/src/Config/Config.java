package Config;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.Calendar;
import java.util.Properties;
import java.util.logging.Level;

//log4j-*.*.**.jar for these imports
import org.apache.log4j.Logger;

//this class manages configuration information for the server
//to retrieve a configuration, use the following syntax
//Config.g().get("port");
//Config.g().set("port", 5);

public class Config
{
	public static int NETWORK_ADAPTER;	//defines which network adapter to use
	public static int PORT_NUM;			//the port number to analyze
	public static String SERVER;			//address of the server to use
	
	private final String SERVER_CONFIG_FILE = "./config/settings.ini";
	
	private static Logger _log = Logger.getLogger(Config.class.getName());
	
	private Config() 
	{
	}
	static private Config _instance;
	//g() is used instead of getInstance() because it is shorter, and will make calling code appear less messy
	static public Config g() 
	{
		if (_instance == null) 
			_instance = new Config();
		return _instance;
	}
	
	public void load() throws Exception
	{	//loads the server configuration
		try 
		{
			Properties serverSettings = new Properties();
			InputStream is = new FileInputStream(new File(SERVER_CONFIG_FILE));
			serverSettings.load(is);
			is.close();
			_log.info("Analyzer configuration loaded");
			NETWORK_ADAPTER = Integer.parseInt(serverSettings.getProperty("WhichAdapter", "0"));
			PORT_NUM = Integer.parseInt(serverSettings.getProperty("Port", "2000"));
			SERVER = serverSettings.getProperty("Server", "207.192.73.73");
		}
		catch (Exception e) 
		{
			_log.fatal(e.getLocalizedMessage());
			throw new Error("Failed to Load " + SERVER_CONFIG_FILE + " File.");
		}
	}
}