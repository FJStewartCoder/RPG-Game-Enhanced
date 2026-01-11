function environment()
    node_1 = build_node("Start", {}, -1, "", false)
    node_2 = build_node("2", {}, node_1, "left", false)
    node_3 = build_node("2", {}, node_2, "up", false)
    node_4 = build_node("2", {}, node_3, "right", false)
end