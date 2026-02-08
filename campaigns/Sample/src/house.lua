function land_house(unique, node, player)
    if unique.name ~= nil then
        print("Welcome to " ..unique.name .."'s house!")
    end

    money_to_steal = 50

    choice = alt_menu("Steal Money", "Would you like to steal any money? ", {"Yes", "No"})

    if choice == "Yes" then
        player.money = player.money + money_to_steal
        player.wanted_level = player.wanted_level + 0.5
    end
end

function leave_house(unique, node, player)
    print("Thank you for visiting!")

    random_number = math.random(0, 1);

    -- 10% chance if you are more than level 5 wanted level to lose
    if random_number < 0.1 and player.wanted_level > 5 then
        print("Your wanted level is too high and you have been found. You lose!")
        quit()
    end
end