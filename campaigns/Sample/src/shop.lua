function land_shop(unique, player)
    print("SHOP: " ..unique.name)

    if ( #unique.items == 0 ) then
        print("Shop has no items")
    end

    item_list = {}

    for i=1, #unique.items do
        item_list[i] = {
            unique.items[i].name,
            "Cost: " ..unique.items[i].cost
        }
    end 
    
    item_list[#item_list+1] = "Quit"

    while true do
        choice = basic_menu("Items", "What would you like to buy?", item_list)

        if choice == "Quit" then
            return
        end

        item_cost = 0

        for i=1, #unique.items do
            if unique.items[i].name == choice then
                item_cost = unique.items[i].cost
                break
            end
        end

        if player.money < item_cost then
            print("You do not have enough money to buy this item")
        else
            if player.inventory[choice] == nil then
                player.inventory[choice] = 1
            else
                player.inventory[choice] = player.inventory[choice] + 1
            end

            print("You bought 1x " ..choice)
            print("You now have " ..player.inventory[choice] .."x " ..choice)

            player.money = player.money - item_cost
        end
    end
end

function leave_shop()
    print("You have left this shop")
end