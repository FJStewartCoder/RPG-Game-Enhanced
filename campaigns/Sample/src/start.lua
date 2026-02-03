function land_start(unique, node, player)
    print("Welcome to the sample game! This is the landing function.")

    if player.name == "Player Name" then
        io.write("Enter your name: ")
        user_name = io.read()

        print( "Hello, " ..user_name .."!" )

        player.name = user_name
    else
        print( "Welcome back " ..player.name .."!" )
    end

    print( "Your current position is: (" ..player.position.x ..", " ..player.position.y ..", " ..player.position.z .. ")" )
end

function leave_start()
    print( "Thank you for visiting the start. This is the leaving function" )
end