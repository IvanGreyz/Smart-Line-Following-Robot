function read_TCPClient(src,evt)
    read_data = read(src,src.NumBytesAvailable,"char");
    data2 = readline(src);
    disp(["Read data" , read_data])
    disp(["Readline data" , data2]);
end