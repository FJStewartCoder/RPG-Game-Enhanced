USE_GENERIC = false
CAMPAIGN_NAME = "Sample Game"

function build()
    add_node({name = "Start", on_land = land_start, on_leave = leave_start})
    add_node({name = "Fight", on_land = land_fight, on_leave = leave_fight})
end

function extend()
    extend_player({health = 100})
end

function environment()
    build_node("Start", 0, 0, 0, {}, "")
    build_node("Fight", 1, 0, 0, {}, "")
end