function val = gui_switch_beginProgram(mdl,simu_status,flag)
    if simu_status
           if flag
                open_system(mdl);
           else
                load_system(mdl);
           end
                val = 'pause';
    else
           if get_param(mdl,'SimulationStatus') ~= "pause"
                set_param(mdl,SimulationCommand="pause");
                val = 'pause';
           end
    end
end