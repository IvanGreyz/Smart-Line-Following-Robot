% Detect ip (optional)
[~,hostname] = system('hostname');
hostname = string(strtrim(hostname));
address = resolvehost(hostname,"address");

% Initialize server
ip = "0.0.0.0"; port = 48561;
server = tcpserver(ip,port,'Timeout',10);
configureTerminator(server,"LF");
configureCallback(server,"terminator",@read_TCPClient);

% ---- Build payload once and store in server.UserData ----
% Build the 5-byte frame: [ int8 | float32(IEEE754, little-endian) ]
spd_i8   = int8(500);           % 1 byte
angle_f4 = single(20);        % 4 bytes

b_speed  = typecast(spd_i8, 'uint8');     % 1x1 uint8
b_angle  = typecast(angle_f4, 'uint8');   % 1x4 uint8 (little-endian on x86)

frame = [b_speed, b_angle];               % 1x5 uint8
server.UserData.frame = frame;

% ---------------- Callback ----------------
function read_TCPClient(src,evt) %#ok<INUSD>
    % How many bytes are waiting (safe & valid for tcpserver)
    disp(src.NumBytesAvailable);

    % Read a line up to LF (as configured)
    txt = readline(src);
    % If client sends CRLF, remove the CR (13), not ETX (3)
    txt = strip(txt, "right", char(13));

    fprintf('[RX]: %s\n', txt);

    % Fetch the prebuilt payload from UserData
    write(src, src.UserData.frame, "uint8");  % send raw bytes

end