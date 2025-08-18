functor
import
    System
    Browser
    OS
define 
    % declare
    % A = 1
    local A in
        thread
            A = 1
        end

        thread
            {System.show value(A)}
        end

        % thread
        %     {Browser.browse A}
        % end

        {System.show {OS.time ?}}
        {System.show {OS.time ?}}
    end
end
