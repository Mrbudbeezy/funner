package com.untgames.funner.facebook_session;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.util.Log;

import com.facebook.AppEventsLogger;
import com.facebook.Session;
import com.facebook.SessionState;
import com.facebook.Settings;

import com.untgames.funner.application.EngineActivity;

public class SessionImpl implements EngineActivity.EngineActivityEventListener, EngineActivity.EngineActivityResultListener
{
  private static final String TAG = "funner.facebook_session.SessionImpl";
  
  private EngineActivity activity;
	private String         applicationId;
	private Session        activeSession;

	public SessionImpl (EngineActivity inActivity, String inApplicationId)
  {
		activity      = inActivity;
		applicationId = inApplicationId;
		
  	activity.addEventListener (this);
  	activity.addResultListener (this);
  }
  
	public void handleEngineActivityEvent (EngineActivity.EventType eventType)
	{
		if (eventType != EngineActivity.EventType.ON_RESUME)
			return;

		publishInstall ();
	}

	public boolean handleEngineActivityResult (int requestCode, int resultCode, Intent data)
	{
	  Session activeSession = Session.getActiveSession ();
	
	  if (activeSession != null)
      activeSession.onActivityResult (activity, requestCode, resultCode, data);
    
    return false;
	}

	//check if we can use sdk login
	public boolean canLogin ()
	{
		String packageName = activity.getPackageName ();
    int    resId       = activity.getResources ().getIdentifier ("FacebookSdkResourcesVersion", "string", packageName);
    
    if (resId == 0)
    {
    	Log.d (TAG, "Application has no Facebook SDK resources, sdk login unavailable");
    	return false;
    }
    
    String resourcesVersion = activity.getString (resId);
    
    return false; //TODO implement custom login
    
/*    Log.d (TAG, "Check can login. SDK version = " + Settings.getSdkVersion () + " resourcesVersion = " + resourcesVersion);

    if (!Settings.getSdkVersion ().equals (resourcesVersion))
    	throw new IllegalStateException ("Facebook SDK and Facebook SDK resources version mismatch");
    
		return true;*/
	}
	
	private boolean isReadPermission (String permission)
	{
		final String [] readPermissions = {
			
		};

		for (String readPermission : readPermissions)
		{
//TODO 			if ()
		}
		
		return false;
	}
	
	private boolean isWritePermission (String permission)
	{
	//TODO
		return false;
	}
	
	public void login (String permissions)
	{
		String [] permissionsArray = permissions.split (",");
		
		List<String> readPermissions  = new ArrayList<String> (permissionsArray.length); 
		List<String> writePermissions = new ArrayList<String> (permissionsArray.length); 
		
		for (String permission : permissionsArray)
		{
			if (isReadPermission (permission))
				readPermissions.add (permission);
			else if (isWritePermission (permission))
				writePermissions.add (permission);
			else
				throw new IllegalArgumentException ("Unknown permission requested: " + permission);
		}
		
		if (!writePermissions.isEmpty ()) //TODO
			throw new RuntimeException ("Write permissions not supported");
		
		activeSession = null;
		
		Log.d (TAG, "Login started");
		
		activity.runOnUiThread (new Runnable () {
  		public void run ()
  		{
  			try
  			{
  				Session.Builder sessionBuilder = new Session.Builder (activity);
  				
  				sessionBuilder.setApplicationId (applicationId);
  				
  				activeSession = sessionBuilder.build ();
  				
  				Session.setActiveSession (activeSession);
  				
  				Session.OpenRequest openRequest = new Session.OpenRequest (activity);
  				
  				openRequest.setCallback (new Session.StatusCallback () {
            // callback when session changes state
            @Override
            public void call(Session session, SessionState state, Exception exception) 
            {
            	//TODO call ok/cancel/error/request permissions
            }
      	  });
  				
      	  activeSession.openForRead (openRequest);
  			}
  			catch (Throwable e)
  			{
  				Log.e (TAG, "Exception while logging in: " + e.getMessage () + "\n stack: ");
  				e.printStackTrace ();
  				//TODO call error
  			}
  		}
		});
	}
	
  public void publishInstall ()
  {
		Log.d (TAG, "Publishing install");

		activity.runOnUiThread (new Runnable () {
  		public void run ()
  		{
  			try
  			{
  				AppEventsLogger.activateApp (activity, applicationId);
  			}
  			catch (Throwable e)
  			{
  				Log.e (TAG, "Exception while publishing install: " + e.getMessage () + "\n stack: ");
  				e.printStackTrace ();
  			}
  		}
  	});
  }
}
