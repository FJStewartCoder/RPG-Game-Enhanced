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
    node_1 = build_node("Start", {}, -1, "", false)
    node_2 = build_node("2", {}, node_1, "left", false)
    node_3 = build_node("2", {}, node_2, "up", false)
    node_4 = build_node("2", {}, node_3, "right", false)
    node_5 = build_node("5", {}, node_4, "right", false)
end