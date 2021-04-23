using System;
using System.Collections;
using System.Collections.Generic;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using System.IO;
using UnityEngine;

public class TCPClient : MonoBehaviour
{

	float x, z, r, px, pz, pr;

	public Transform buggy;
	public TextMesh textBox;

	
	private TcpClient socketConnection;
	private Thread clientReceiveThread;
	private NetworkStream stream;
	private StreamReader sr;
	private StreamWriter sw;
	private string serverMessage = "0/0/0";
	private bool isMoving = false;

	// Use this for initialization 	
	void Start()
	{
		socketConnection = new TcpClient();
	}
	// Update is called once per frame
	void Update()
	{
		if (Input.GetKeyDown("g"))
		{
			TCPSendMessage("g");
			isMoving = true;
		}

		if (Input.GetKeyDown("s"))
		{
			TCPSendMessage("s");
		}

		if (Input.GetKeyDown("c"))
		{
			ConnectToTcpServer();
		}

		if (Input.GetKeyDown("d"))
		{
			TCPDisconnect();
		}

		if (socketConnection.Connected) {
			if (stream.DataAvailable)
			{
				MoveBuggy(TCPReadMessage());
				if (isMoving == true) {
					textBox.text = "Position x : " + x + " y: "+ z + " with rotation: " + r + "degrees";
				}
			}
			else
			{

				Debug.Log("Stream empty");

			}
		}

	}

	private void ConnectToTcpServer()
	{

		try
		{

			Debug.Log("Connecting...");
			socketConnection.Connect("192.168.50.134", 8080);
			Debug.Log("Connected");
			textBox.text = "Connected. Press G to start moving";

			stream = socketConnection.GetStream();
			sr = new StreamReader(stream);
			sw = new StreamWriter(stream);

		}
		catch (SocketException e) {
			Debug.Log("SocketException: " + e);
		}


		//	StartTCPReaderThread();

	}

	private void TCPDisconnect()
	{
		Debug.Log("Disconnecting...");
		socketConnection.Close();
		Debug.Log("Disconnected");
		socketConnection = new TcpClient();
	}

	private string TCPReadMessage()
	{

		// Get a stream object for reading 				

		Debug.Log("Reading message");

		serverMessage = sr.ReadLine();

		return (serverMessage);

	}

	private void TCPSendMessage(string message)
	{
		if (socketConnection == null)
		{
			return;
		}

		if (stream.CanWrite)
		{
			Debug.Log("Writing message");
			string clientMessage = message;
			sr.DiscardBufferedData();
			sw.WriteLine(message);
			sw.Flush();
		}
		
	}

	private void MoveBuggy(string positionInfo) {


		Debug.Log(positionInfo);

		string returndata = positionInfo.Trim();

		string[] items = returndata.Split('/');

		if (items.Length > 1)
		{
			try
			{

				x = float.Parse(items[0]);
				z = float.Parse(items[1]);
				r = float.Parse(items[2]);

				Debug.Log("moving " + x + " / " + z + " / " + r);

				px = x;
				pz = z;
				pr = r;

			}
			catch (FormatException)
			{
				Debug.Log("Wrong Format");
				x = px;
				z = pz;
				r = pr;
			}


			buggy.position = new Vector3(x, 0, z);
			buggy.rotation = Quaternion.Euler(0, 180 + r, 0);

			Debug.Log(x + " / " + z + " / " + r);

		}

	}

	// Thread scheduling is pretty shit in unity so that doesnt work that well

	private void StartTCPReaderThread()
	{

		clientReceiveThread = new Thread(new ThreadStart(TCPReadMessageThread));
		clientReceiveThread.IsBackground = true;
		clientReceiveThread.Start();

	}

	private void TCPReadMessageThread()
	{

		while (true)
		{
			// Get a stream object for reading 				
			if (socketConnection.Connected)
			{

				Debug.Log("Reading message");

				serverMessage = sr.ReadLine();

				Debug.Log(serverMessage);

			}
			else
			{
				Debug.Log("Disconnected. Reconnecting...");
				socketConnection.Connect("192.168.50.134", 8080);
			}
		}

	}

}