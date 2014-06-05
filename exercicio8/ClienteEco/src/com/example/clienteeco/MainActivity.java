package com.example.clienteeco;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintWriter; 
import java.io.OutputStreamWriter;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.net.Socket;
import java.net.UnknownHostException;

import android.support.v7.app.ActionBarActivity;
import android.support.v7.app.ActionBar;
import android.support.v4.app.Fragment;
import android.os.Bundle;
import android.view.LayoutInflater;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.view.ViewGroup;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.os.Build;

import android.os.AsyncTask;
import android.app.Activity;
import android.util.Log;


public class MainActivity extends ActionBarActivity
{	
	TextView textResponse;
	EditText editTextAddress, editTextPort,editTextMsg; 
	Button buttonConnect, buttonEnviar, buttonClear;

	@Override
	protected void onCreate(Bundle savedInstanceState)
	{
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		editTextAddress = (EditText)findViewById(R.id.editTextAddress);
		editTextPort = (EditText)findViewById(R.id.editTextPort);
		buttonConnect = (Button)findViewById(R.id.buttonConnect);
		buttonClear = (Button)findViewById(R.id.buttonClear);
		textResponse = (TextView)findViewById(R.id.textResponse);
		
		/*Sent Button and Message Text Field*/
		buttonEnviar = (Button)findViewById(R.id.buttonEnviar);
		editTextMsg = (EditText)findViewById(R.id.editTextMsg);
		
		buttonConnect.setOnClickListener(buttonConnectOnClickListener);
		
		buttonClear.setOnClickListener(new OnClickListener()
		{
			@Override
			public void onClick(View v)
			{
				textResponse.setText("");
			}
		});
	}
	
	OnClickListener buttonConnectOnClickListener = new OnClickListener()
	{

		@Override
		public void onClick(View arg0)
		{
			MyClientTask myClientTask = new MyClientTask(editTextAddress.getText().toString(), Integer.parseInt(editTextPort.getText().toString()));
			myClientTask.execute();
		}
	};

	@Override
	public boolean onCreateOptionsMenu(Menu menu) {

		// Inflate the menu; this adds items to the action bar if it is present.
		getMenuInflater().inflate(R.menu.main, menu);
		return true;
	}

	@Override
	public boolean onOptionsItemSelected(MenuItem item) {
		// Handle action bar item clicks here. The action bar will
		// automatically handle clicks on the Home/Up button, so long
		// as you specify a parent activity in AndroidManifest.xml.
		int id = item.getItemId();
		if (id == R.id.action_settings) {
			return true;
		}
		return super.onOptionsItemSelected(item);
	}

	/**
	 * A placeholder fragment containing a simple view.
	 */
	public static class PlaceholderFragment extends Fragment {

		public PlaceholderFragment() {
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container,
				Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.fragment_main, container,
					false);
			return rootView;
		}
	}
	
	
	public class MyClientTask extends AsyncTask<Void, Void, Void>
	{
		String dstAddress;
		int dstPort;
		String response = "";
		
		MyClientTask(String addr, int port)
		{
			dstAddress = addr;
			dstPort = port;
		}

		@Override
		protected Void doInBackground(Void... arg0) 
		{
			Socket socket = null;
			
			try
			{
				socket = new Socket(dstAddress, dstPort);
			
				response = "Conectado";
				publishProgress();
				
				InputStream inputStream = socket.getInputStream();
				BufferedReader entrada = new BufferedReader(new InputStreamReader(inputStream));
			
				OutputStream outputStream= socket.getOutputStream();
				PrintWriter saida = new PrintWriter(new OutputStreamWriter(outputStream),true);
				
				saida.println("Sou eu!");
				
				/*
					* notice:
					* entrada.readLine() will block if no data return
					*/  
				
				String datos = entrada.readLine();
				response = datos.toString();
				
			} 
			catch (UnknownHostException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
				response = "UnknownHostException: " + e.toString();
			}
			catch (IOException e)
			{
				// TODO Auto-generated catch block
				e.printStackTrace();
				response = "IOException: " + e.toString();
			}
			finally
			{
				if(socket != null)
				{
					try 
					{
						Log.d("Dev", "Close Socket");
						socket.close();
					}
					catch (IOException e)
					{
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
			return null;
		}

		@Override
		protected void onPostExecute(Void result)
		{
			textResponse.append(response);
			textResponse.append("\n");
			super.onPostExecute(result);
		}
		
		@Override
		protected void onProgressUpdate (Void... v)
		{
			super.onProgressUpdate(v);
			textResponse.append(response);
			textResponse.append("\n");
			Log.d("Dev", "Estou na funcao onProgressUpdate");
		}
		
	}

}
