package Config;

//log4j-*.*.**.jar for these imports
import org.apache.log4j.Logger;

public class Shutdown extends Thread {
	private static Logger _log = Logger.getLogger(Shutdown.class.getName());
	private static Shutdown _instance;

	private int secondsShut;

	private int shutdownMode;
	public static final int SIGTERM = 0;
	public static final int SHUTDOWN = 1;
	public static final int RESTART = 2;
	public static final int ABORT = 3;
	private static String[] _modeText = { "SIGTERM", "shutting down", "restarting", "aborting" };

	public Shutdown() 
	{
		secondsShut = 30;	//30 seconds to shutdown
		shutdownMode = SIGTERM;
	}

	public Shutdown(int seconds, boolean restart) 
	{
		if (seconds < 0) 
		{
			seconds = 0;
		}
		secondsShut = seconds;
		if (restart) 
		{
			shutdownMode = RESTART;
		}
		else
		{
			shutdownMode = SHUTDOWN;
		}
	}

	public static Shutdown getInstance() 
	{
		if (_instance == null) 
		{
			_instance = new Shutdown();
		}
		return _instance;
	}
	
	private void setTime(int seconds)
	{
		if (seconds < 0) 
		{
			seconds = 0;
		}
		secondsShut = seconds;
	}
	private void setMode(int mode) 
	{
		shutdownMode = mode;
	}
	private int getMode() 
	{
		return shutdownMode;
	}
	private void _abort() 
	{
		shutdownMode = ABORT;
	}


	@Override
	public void run()
	{
		if (this == _instance)
		{	//this is the shutdown hook instance
			//this log probably wont output because the Logger also has a shutdown hook
			if (shutdownMode == SIGTERM)
			{	//only display this for the unknown shutdowns
				_log.warn("The analyzer is shutting down for an unknown reason.");
				System.out.println("The analyzer is shutting down for an unknown reason.");
				System.err.println("The analyzer is shutting down for an unknown reason.");
			}
		}
		else 
		{	//not the shutdown hook instance
			switch (shutdownMode) 
			{
				case SHUTDOWN:
					_log.warn("The analyzer is being shut down.");
					_instance.setTime(0);
					_instance.setMode(SHUTDOWN);
					System.exit(0);	//calls the shutdown hooks
					break;
				case RESTART:
					_log.warn("The analyzer is being restarted.");
					_instance.setTime(0);
					_instance.setMode(RESTART);
					System.exit(1);	//calls the shutdown hooks
					break;
			}
		}
	}
	
	private void countdown()
	{
		try {
			while (secondsShut > 0) {
				//process and alert players that server is on a timer to shutdown/restart
				secondsShut--;

				int delay = 1000; // milliseconds
				Thread.sleep(delay);

				if (shutdownMode == ABORT) {
					break;
				}
			}
		} catch (InterruptedException e) {
			_log.warn("The countdown was interrupted by an exception");
			System.err.println("The countdown was interrupted by an exception");
			 Thread.currentThread().interrupt();
		}
	}

	
}