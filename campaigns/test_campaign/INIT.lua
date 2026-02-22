MODULES = {
    "abc",
    "123"
}

function build()
    -- do_something_funny()

    new_node_type("Start", start, start_leave, {})
    new_node_type("2", function () end, function () end, {})
    new_node_type("3", function () end, function () end, {})
    new_node_type("4", function () end, function () end, {})

    new_node_type(
        "Test",
        function () end,
        function () end,
        {
            a = "a",
            b = "b",
            c = "c",
            d = "d",
            e = {
                f = "f",
                g = "g"
            }
        }
    )

    return 0
end

function extend()

    extend_player({
        f = function () end,
        name = "Different Name"
    })

end

function environment()
    -- TEST IS BLOCKING WORKS ON ALL DIRECTIONS
    build_node("Start", "The Beginning", {0, 0, 0}, {}, "rudfbnp")
    build_node("2", "", {1, 0, 0}, {}, "")
    build_node("2", "", {-1, 0, 0}, {}, "")
    build_node("2", "", {0, 1, 0}, {}, "")
    build_node("2", "", {0, -1, 0}, {}, "")
    build_node("2", "", {0, 0, 1}, {}, "")
    build_node("2", "", {0, 0, -1}, {}, "")

    -- max and min are 32767 -32768
    build_node("2", "", {32767, 32767, 32767}, {}, "")
    -- too big
    -- build_node("2", "", {32768, 32768, 32768}, {}, "")
    build_node("2", "", {-32768, -32768, -32768}, {}, "")
    -- too small
    -- build_node("2", "", {-32769, -32769, -32769}, {}, "")

    make_connection(
        {0, 0, 0},
        {0, 1, 0},
        "next",
        true,  --one way
        true  --bypass blocks
    )

    -- create the other nodes
    blank()
end 

function blank()
    --  2 
    -- 2S2
    --  2

    -- Example 1 
    -- Typical use

    -- EXPECTED:
    -- normal

    -- ACTUAL:
    -- normal
    build_node(
        "Test",
        "",
        {1, 2, 0},
        {
            a = "A",
            b = "B",
            c = "C",
            d = "D",
            e = {
                f = "F",
                g = "G"
            }
        },
        ""
    )

    -- Example 2
    -- Alternate but typical

    -- EXPECTED:
    -- normal but could choose random order for keys

    -- ACTUAL:
    -- the keys decided upon a random order as expected
    build_node(
        "Test",
        "",
        {2, 2, 0},
        {
            "A",
            "B",
            "C",
            "D",
            {
                "F",
                "G"
            }
        },
        ""
    )

    -- Example 3
    -- Show template can be combined in any order and will default values

    -- EXPECTED:
    -- b and d will be lowercase ( defaults will be filled in )

    -- ACTUAL:
    -- as expected
    build_node(
        "Test",
        "",
        {3, 2, 0},
        {
            c = "C",
            a = "A",
            e = {
                f = "F"
            }
        },
        ""
    )

    -- Example 4
    -- Show defaults work when using list args

    -- EXPECTED:
    -- d will be lowercase and e will be list

    -- ACTUAL:
    -- defaults were filled in for the missing fields but fields were in random order
    build_node(
        "Test",
        "",
        {4, 2, 0},
        {
            "A",
            "B",
            "C"
        },
        ""
    )

    -- Example 5
    -- Show you can't add new data

    -- EXPECTED:
    -- new values are invalid and all data is uppercase

    -- ACTUAL:
    -- as expected
    build_node(
        "Test",
        "",
        {5, 2, 0},
        {
            a = "A",
            b = "B",
            c = "C",
            d = "D",
            e = {
                f = "F",
                g = "G"
            },
            h = "H",
            i = "I"
        },
        ""
    )

    -- Example 6
    -- Show you can't add new data using list args

    -- EXPECTED:
    -- doesn't accept too many

    -- ACTUAL:
    -- as expected but scrambled keys and list is list not table but this is as intended
    build_node(
        "Test",
        "",
        {6, 2, 0},
        {
            "A",
            "B",
            "C",
            "D",
            {
                "F",
                "G"
            },
            "H",
            "I"
        },
        ""
    )

    -- Example 7
    -- Show that the template is not type safe

    -- EXPECTED:
    -- types are overwriten

    -- ACTUAL:
    -- as expected
    build_node(
        "Test",
        "",
        {7, 2, 0},
        {
            a = 1,
            b = 2,
            e = 10
        },
        ""
    )

    -- Example 8
    -- Show that tables are not deep combined

    -- EXPECTED:
    -- defaults work and table is infact a copy 

    -- ACTUAL:
    -- as expected
    build_node(
        "Test",
        "",
        {8, 2, 0},
        {
            e = {
                f = "F",
                i = "I",
                h = "H"
            }
        },
        ""
    )
end