package Analyzer;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.sql.Connection;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Collection;
import java.util.logging.Level;

//log4j-*.*.**.jar for these imports
import org.apache.log4j.Logger;

import Config.Config;

public class ListenUpdate extends Thread
{
	private ServerSocket _serverSocket;
	private static Logger _log = Logger.getLogger(ListenUpdate.class.getName());
	private int _port;
	
	public ListenUpdate(ServerSocket socket)
	{
		_serverSocket = socket;
		_port = socket.getLocalPort();

	}
			
	@Override
	public void run() 
	{
		_log.info("Listening for connections on port " + _port);
		while (true) 
		{
			try 
			{
				Socket socket = _serverSocket.accept();
				_log.info("Connection accepted from IP: "+ socket.getInetAddress());
				String host = socket.getInetAddress().getHostAddress();
				socket.setTcpNoDelay(true);	//disable Nagle's algorithm
				
				//TODO: make another class for update conversations
				Thread new_client = new Thread(new ConversationUpdate(socket));
				new_client.start();
			}
			catch (IOException ioexception) 
			{
				_log.warn("Exception:" + ioexception);
			}
		}
	}
}