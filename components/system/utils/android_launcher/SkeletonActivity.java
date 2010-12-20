package com.untgames.android.funner_launcher;

import android.app.Activity;
import android.os.Bundle;
import android.util.*;
import java.io.*;

/// ������ ����� ������������ ��� ������� ������� shared-library
public class SkeletonActivity extends Activity
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
}
