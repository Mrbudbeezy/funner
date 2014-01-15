package com.untgames.funner.push_notifications;

import android.content.Context;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.util.Log;

import com.google.android.gcm.GCMRegistrar;

//����� ��� ������ � ����������� google cloud messaging
public class EngineGCMManager
{
  private static final String TAG = "EngineGCMManager";

  //��������� �������������� �����������
  public static String getSenderId(Context context) {
  		String packageName = context.getPackageName();
      int    resId       = context.getResources().getIdentifier("GCMSenderID", "string", packageName);

      if (resId == 0)
  		{
  			Log.e (TAG, "'GCMSenderID' metadata not setted in application manifest");
  			return null;
  		}

  		return context.getString(resId);
  }
	
	//����������� �� ��������� ���������
	public static void registerForGCMMessages (Context context)
	{
		try
		{
  		GCMRegistrar.checkDevice(context);
	  	GCMRegistrar.checkManifest(context);
		}
		catch (Exception exception)
		{
			onErrorCallback ("Can't register for gcm, exception " + exception);
			return;
		}
		
		final String regId = GCMRegistrar.getRegistrationId(context);

		if (regId.equals("")) {
		  GCMRegistrar.register(context, getSenderId (context));
		} else {
			onRegisteredCallback(regId);
		}
  }

  public static void unregisterForGCMMessages (Context context)
  {
    GCMRegistrar.unregister (context);
  }

  private static native void onRegisteredCallback(String request);
  private static native void onErrorCallback(String error);
}
