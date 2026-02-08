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

    checking_status = true

    while checking_status do
        choice = basic_menu("Status", "Status", {"Health", "Position", "Quit"})

        if choice == "Health" then
            print( "Your current health is " ..player.health .."hp." )
        elseif choice == "Position" then
            print( "Your current position is: (" ..player.position.x ..", " ..player.position.y ..", " ..player.position.z .. ")" )
        else
            checking_status = false 
        end
    end 
end

function leave_start()
    print( "Thank you for visiting the start. This is the leaving function" )
end