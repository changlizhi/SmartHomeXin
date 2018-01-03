#include <stdio.h>
#include "ssmtp.h"

int SendAlarmMail(char * fromaddr,char *username,char*pwd,char *to,char *content,char *picdir)
{

     SMTP_Client_Info client;
     SMTP_Mail_Info mail;
     initClientInfo(&client);
     initMailInfo(&mail );

     client.host = "smtp.163.com";
     client.fromaddr = fromaddr;//"15012890652@163.com";
     client.fromName = "SmartHome";
     client.auth_user = username;//"15012890652";
     client.auth_pass = pwd;//"sg19860208";
     client.auth_mech = 0 ;

     mail.to = to;//"chenshugao@jhw.com.cn";
     mail.subject = "alarm form smarthome,please check you room";
     //mail.body = "something has happed ,\n,please check you room!";
     mail.body = content;//"make a alarm please check you room";
     mail.attachment = picdir;//"abc.jpg";

     int i = sendMail(&client , &mail);
     if (!i)
     {
         printf("mail has sent!\n");

     }else{
         printf("mail send error!,%d\n",i);
     }

        return i;
}
