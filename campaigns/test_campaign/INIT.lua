--[[
MODULES = {
    "abc",
    "123"
}
]]

function build()
    -- do_something_funny()

    new_node_type("Start", start, start_leave, {})
    new_node_type("2", function () end, function () end, {})
    new_node_type("3", function () end, function () end, {})
    new_node_type("4", function () end, function () end, {})

    return 0
end

function extend()
    extend_player({f = function ()
        
    end})
end

function environment()
    -- TEST IS BLOCKING WORKS ON ALL DIRECTIONS
    build_node("Start", "", 0, 0, 0, {}, "rudfbnp")
    build_node("2", "", 1, 0, 0, {}, "")
    build_node("2", "", -1, 0, 0, {}, "")
    build_node("2", "", 0, 1, 0, {}, "")
    build_node("2", "", 0, -1, 0, {}, "")
    build_node("2", "", 0, 0, 1, {}, "")
    build_node("2", "", 0, 0, -1, {}, "")

    make_connection(0, 0, 0, 0, 1, 0, "next", false, true)

    --  2 
    -- 2S2
    --  2
end