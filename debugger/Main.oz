functor
import
    OS
    Open
    System
define 
    Client = {New Open.socket init}
in
    try
        {Client connect(host:"127.0.0.1" port:51966)}
        {Client write(vs:"This is a command from client")}
    catch E then
        {System.show exception(E)}
    finally
        {Client close}
    end
end