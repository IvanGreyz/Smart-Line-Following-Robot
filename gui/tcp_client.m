client = tcpclient("169.254.166.209",55001);
write(client,sprintf("Hello\n"),"char")
pause(1);
write(client,sprintf("Hello\n"),"char")