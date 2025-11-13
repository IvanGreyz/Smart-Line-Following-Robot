function mes = gui_logout_str(str)
    if ~isstring(str)
        disp("Error when log out message!")
        return;
    end
    mes = sprintf(string(datetime('now','Format','HH:mm:ss'))+": "+str+"\n\n");
end

