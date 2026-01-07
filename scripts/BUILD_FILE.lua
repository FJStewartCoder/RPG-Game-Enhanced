function build()
    add_node({name = "Start", on_land = start, on_leave = start_leave, random = 2})
    add_node({name = "2", on_land = function () end, on_leave = function () end})
    add_node({name = "3", on_land = function () end, on_leave = function () end})
    add_node({name = "4", on_land = function () end, on_leave = function () end})

    return 0
end

function extend()
    extend_node({bob = "abc"})
end