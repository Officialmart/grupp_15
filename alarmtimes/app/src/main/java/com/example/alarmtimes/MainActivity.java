package com.example.alarmtimes;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

    static int amount = 0;  //en klassvariabel för att hålla koll på hur många tider som ska skrivas in. Den är även en klassvariabel för att kunna anändas från nästa aktivitet
    Button btn;
    String time = "";
    static ArrayList<String> times = new ArrayList<String>();   //en arraylist som är en dynamisk array. I denna lagras tiderna som atas in. Den är klassvariabel för att kunna kommas åt från alla akiviteter

    //när appen startas startas denna aktivitet och visar main desiginen
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        btn = (Button) findViewById(R.id.button3);
        disableButton();//inaktiverar knappen för att siffran är 0
    }

    public void numbsub()//metod för att endast minska antalet tider kvar att skriva i nästa aktivitet
    {
        amount--;
    }

    //metod för att öka antalet inmatningsfält som anropas av + knappen
    public void addNumb(View v)
    {
        if (amount <= 9)
        {
            amount++;
        }
        display(amount);
        if (amount >= 1)
        {
            enableButton();
        }
        else
        {
            disableButton();
        }
    }

    //metod för att minska antalet inmatningsfält som anropas av - knappen
    public void subNumb(View v)
    {
        if (amount >= 1)
        {
            amount--;
        }
        display(amount);
        if (amount >= 1)
        {
            enableButton();
        }
        else
        {
            disableButton();
        }
    }

    //aktiverar gå vidare knappen
    public void enableButton()
    {
        btn.setEnabled(true);
    }

    //inaktiverar gå vidare knappen
    public void disableButton()
    {
        btn.setEnabled(false);
    }

    //uppdaterar textfältet med siffran
    public void display(int amount)
    {
        TextView textView = (TextView) findViewById(R.id.textView3);
        textView.setText("" + amount);

        System.out.println("" + amount);
    }

    //metod för att gå vidare till nästa aktivitet för att mata in tider
    public void showNext(View v)
    {
        Intent intent = new Intent(this, Inputtime.class);
        startActivity(intent);
    }

    //skriver ut tiderna med eget format
    public String printList()
    {
        String result = "";

        for (int i = 0; i < times.size(); i++)
        {
            result = result + times.get(i);
            if (i != times.size())
            {
                result = result + " ";
            }
        }

        result = result + "\n";

        return result;
    }
}


