USE_GENERIC = true

function build()
    do_something_funny()

    add_node({name = "Start", on_land = start, on_leave = start_leave, random = 2})
    add_node({name = "2", on_land = function () end, on_leave = function () end})
    add_node({name = "3", on_land = function () end, on_leave = function () end})
    add_node({name = "4", on_land = function () end, on_leave = function () end})

    return 0
end

function extend()
    extend_node({bob = "abc"})
end

function environment()
    node_1 = build_node("Start", {}, {}, "")
    node_1 = build_node("2", {}, {}, "")
    node_1 = build_node("3", {}, {}, "")
    node_1 = build_node("2", {}, {}, "")
    node_1 = build_node("5", {}, {}, "")
end