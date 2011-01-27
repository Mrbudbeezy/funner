package com.untgames.funner.application;

import android.app.Activity;
import android.view.View;
import android.view.ViewGroup;
import android.os.Bundle;
import android.os.Looper;
import android.util.*;
import java.io.*;

/// ������ ����� ������������ ��� ������� ������� shared-library
public class EngineActivity extends Activity
{
///���������
  @Override
  public void onCreate(Bundle savedInstanceState)
  {
      /// ��������� ���������� �������
    
    Bundle extras = getIntent ().getExtras ();
    
    if (extras == null)
    {
      System.out.println ("No command line arguments found");
      System.exit (0);
      
      return;
    }
    
    String programName = extras.getString ("program");
    
    if (programName == null)
    {
      System.out.println ("No 'program' command line argument found");
      System.exit (0);
      
      return;
    }

    String programArgs = extras.getString ("args");
    
      //�������� ���������� native ����
    
    try
    {
      if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.GINGERBREAD)
        System.loadLibrary ("android");
    	
      System.load (programName);

      if (startApplication (programName, programArgs != null ? programArgs : "") == 0)
        System.exit (0);
    }
    catch (Throwable e)
    {
      System.out.println (e.getMessage());
      e.printStackTrace ();
      
      System.exit (0);
    }       
    
    super.onCreate (savedInstanceState);
  }
  
/// ����� ����� � native ���
  public native int startApplication (String programName, String programArgs);
  
/// �������� ����
  public View createEngineView (String initString)
  {
    final Activity activity = this;
    
    return (View)UiDispatch.run (this, new UiRunnable () {
      public Object run ()
      {
        View view = new EngineView (activity);
        
        getWindow ().addContentView (view, new ViewGroup.LayoutParams (ViewGroup.LayoutParams.WRAP_CONTENT, ViewGroup.LayoutParams.WRAP_CONTENT));        
        
        return view;
      }
    });
  }
}
