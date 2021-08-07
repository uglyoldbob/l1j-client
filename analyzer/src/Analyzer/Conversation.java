package Analyzer;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.lang.InterruptedException;
import java.lang.Thread;
import java.net.Socket;
import java.util.Random;

//import jpcap.*;
//import jpcap.packet.Packet;
//import jpcap.packet.TCPPacket;

import Config.Config;
import encryptions.ClientIdExistsException;
import encryptions.LineageEncryption;
import encryptions.LineageKeys;
import types.UByte8;
import types.UChar8;


//log4j-*.*.**.jar for these imports
import org.apache.log4j.Logger;

class Conversation implements Runnable
{	//follows conversations, one of these per client/server connection
	private static Logger _log = Logger.getLogger(Conversation.class.getName());
	
	//these are used for direct communication with the mentioned element
	private Socket _client_me;
	private Socket _server_me;
	
	//these form the two directions of communications and have their own threads
	public ClientServer _cs;
	public ServerClient _sc;

	public Conversation(Socket from_client) throws IOException 
	{	//this is passed a socket to be used with the client
		//might possibly need 3 threads
			//one for client to server passing
			//another for server to client passing
			//and a third for doing other stuff
			
		_client_me = from_client;	//save the socket for communication with the client	
		
		//open a connection to the server		
		_server_me = new Socket(Config.SERVER, Config.PORT_NUM);
		
		_cs = new ClientServer(_client_me.getInputStream(), _server_me.getOutputStream(), this);
		Thread temp = new Thread(_cs);
		temp.start();
		
		_sc = new ServerClient(_server_me.getInputStream(), _client_me.getOutputStream(), this);
		temp = new Thread(_sc);
		temp.start();

	}
	
	private void sendPacket(OutputStream send_to, LineageKeys decKey, int use_key, byte[] abyte0) 
	{
		synchronized (this) 
		{
			try 
			{
				//byte abyte0[] = packet.getContent();
//				_log.info("Send " + abyte0[0] + " to " + getAccountName() + ":" + _hostname);
				char ac[] = new char[abyte0.length];
				ac = UChar8.fromArray(abyte0);
				if (use_key == 1)
					ac = LineageEncryption.encrypt(ac, decKey);
				abyte0 = UByte8.fromArray(ac);
				int j = abyte0.length + 2;

				send_to.write(j & 0xff);
				send_to.write(j >> 8 & 0xff);
				send_to.write(abyte0);
				send_to.flush();
			}
			catch (Exception e) 
			{
			}
		}
	}


	
/*	private byte[] readPacket(InputStream from_here, int dataLength, LineageKeys enkey) throws Exception 
	{	//client = 1 means this packet came from the client
		//0 means it came from the server
		try 
		{
			byte data[] = new byte[dataLength];

			int readSize = 0;

			for (int i = 0; i != -1 && readSize < dataLength; readSize += i) 
			{
				i = from_here.read(data, readSize, dataLength - readSize);
			}

			if (readSize != dataLength) 
			{
				_log.warn("Incomplete Packet is sent to the server, closing connection.");
				throw new RuntimeException();
			}
			if ((_ckeys_initialized != 0) && (client == 1))
			{
				int opcodeP = data[0] & 0xFF;
				byte [] decrypted = LineageEncryption.decrypt(data, dataLength, _clkey);
				int opcode = decrypted[0] & 0xFF;
				printBuffer(decrypted);
//				_log.info("Receive D " + opcode);// + " from " + getAccountName() + ":" + _hostname);
				return decrypted;
			}
			else if ((_skeys_initialized != 0) && (client == 0))
			{
				int opcodeP = data[0] & 0xFF;
				byte [] decrypted = LineageEncryption.decrypt(data, dataLength, _slkey);
				int opcode = decrypted[0] & 0xFF;
				printBuffer(decrypted);
//				_log.info("Receive D " + opcode);// + " from " + getAccountName() + ":" + _hostname);
				return decrypted;
			}
			else if ((data[0] == 50) && (client == 0))
			{	//encryption key time
				long add = data[1] & 0xFF;
				_seed += add;
				add = data[2] & 0xFF;
				_seed += add * 0x100;
				add = data[3] & 0xFF;
				_seed += add * 0x10000;
				add = data[4] & 0xFF;
				_seed += add * 0x1000000;
				_log.info("Encryption key: " + _seed);
				initEncryption(_seed, _server_to_client, 0);
				initEncryption(_seed, _client_to_server, 1);
				_keys_initialized = 1;
			}
			int opcode = data[0] & 0xFF;
			_log.info("Receive P " + opcode);// + " from " + getAccountName() + ":" + _hostname);
			printBuffer(data);
			return data;
		} 
		catch (IOException e) 
		{
			throw e;
		}
	}*/
	
	private void printBuffer(byte[] printme)
	{	//logs the contents of a buffer
		_log.info(toHexString(printme));
	
	/*	for (int counter = 0; counter < printme.length; counter += 16)
		{
			String row_output = " ";
			int row_amount = printme.length - counter;
			for (int tally = 0; tally < row_amount; tally++)
			{
				int sampling = printme[counter+tally] & 0xFF;
				row_output = row_output + sampling + " ";
			}
			_log.info(row_output);
		}*/
	}

private char[] hexChar = {
   '0' , '1' , '2' , '3' ,
   '4' , '5' , '6' , '7' ,
   '8' , '9' , 'a' , 'b' ,
   'c' , 'd' , 'e' , 'f'};
		
private String toHexString ( byte[] b )
{
	StringBuffer sb = new StringBuffer( b.length * 2 );
	for ( int i=0; i<b.length; i++ )
		{
		// look up high nibble char
		sb.append( hexChar [( b[i] & 0xf0 ) >>> 4] );

		// look up low nibble char
		sb.append( hexChar [b[i] & 0x0f] );
		sb.append(' ');
		}
	return sb.toString();
}

	public void closes()
	{
		try
		{
			_server_me.close();
		}
		catch (Exception e)
		{
			_log.warn(e.getLocalizedMessage());
		}
	}
	
	public void closec()
	{
		try
		{
			_server_me.close();
		}
		catch (Exception e)
		{
			_log.warn(e.getLocalizedMessage());
		}
	}
	
	public void run()
	{	//here is where the main work will be done for this conversation
		while (true)
		{
			try 
			{ 
				Thread.sleep ( 1000 ); 
			}  
			catch (InterruptedException e) 
			{
				 
			} 

		}
	}
}

