function environment()
    node_1 = build_node("Start", {}, -1, "", false)
    print(node_1)

    node_2 = build_node("2", {}, node_1, "left", false)
    print(node_2)

    node_3 = build_node("2", {}, node_2, "up", false)
    print(node_3)

    node_4 = build_node("2", {}, node_3, "right", false)
    print(node_4)
end

--    2 3
--    1 0