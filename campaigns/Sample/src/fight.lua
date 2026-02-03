function land_fight(unique, node, player)
    print("You have landed on a fight")

    while true do
        io.write("Would you like to fight the dragon? ")
        ans = io.read()

        if ans == "y" then
            print("You chose to fight the dragon")

            print("The dragon hit you!")

            player.health = player.health - 10

            print("You now have " ..player.health .."hp")

            if player.health <= 0 then
                print("You died")
                quit()
            end

            break
        elseif ans == "n" then
            print("You chose not to fight the dragon")
            break
        else
            print("Please enter either y or n")
        end
    end
end

function leave_fight()
    print("You have left the fight")
end