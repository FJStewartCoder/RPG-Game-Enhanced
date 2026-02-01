USE_GENERIC = true

function abc()
    print("ABC")
end

function build()
    add_node({name = "ABC", on_land = abc, on_leave = abc, random = 2})

    return 0
end

function extend()
    extend_node({bob = "abc"})
end

function environment()
    node_1 = build_node("ABC", {}, -1, "", false)
    node_2 = build_node("ABC", {}, node_1, "left", false)
    node_3 = build_node("ABC", {}, node_2, "up", false)
    node_4 = build_node("ABC", {}, node_3, "right", false)
    node_5 = build_node("ABC", {}, node_4, "right", false)
end