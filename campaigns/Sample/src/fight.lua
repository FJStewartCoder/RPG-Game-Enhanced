function land_fight(unique, node, player)
    print("You have landed on a fight")
    
    -- if the dragon boss is already defeated, return
    if player.bosses_defeated.dragon == true then
        print("You have already killed the dragon")
        return
    end

    while true do
        io.write("Would you like to fight the dragon? ")
        ans = io.read()

        if ans == "y" then
            print("You chose to fight the dragon")

            has_sword = player.inventory["Sword"] ~= nil

            if not has_sword then
                print("The dragon hit you!")

                player.health = player.health - 10

                print("You now have " ..player.health .."hp")

                if player.health <= 0 then
                    print("You died")
                    quit()
                end
            else 
                print("You killed the dragon with your sword")
                -- set dragon to killed
                player.bosses_defeated.dragon = true
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