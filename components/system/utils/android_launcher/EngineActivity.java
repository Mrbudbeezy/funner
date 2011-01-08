package com.untgames.funner.application;

import android.app.Activity;
import android.view.View;
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
  public View createCustomView (String initString)
  {
    try
    {
//      Looper.prepare ();  
        
      return new EngineView (this);
    }
    catch (Exception e)
    {
      System.out.println (e.getMessage ());      
    }
    
    return null;    
  }
}
