unless ARGV.size == 3
    puts 'Usage: ruby test_gen.rb categories=<num categories> orders=<num orders> users=<num users>'
    exit(1)
end

ROOT = File.expand_path(File.dirname(__FILE__))

class User

    attr_accessor :name, :address, :state, :zip, :credit, :id

    def initialize(name, address, state, zip, credit, id)
        @name = name
        @address = address
        @state = state
        @zip = zip
        @credit = credit
        @id = id
    end
end

class Order

    attr_accessor :title, :price, :category, :id

    def initialize(title, price, category, id)
        @title = title
        @price = price
        @category = category
        @id = id
    end
end

ARGV.each do |arg|
    num = arg[(arg.index('=') + 1)..-1]
    if arg.include?('categories')
        NUM_CATEGORIES = num.to_i
    elsif arg.include?('users')
        NUM_USERS = num.to_i
    elsif arg.include?('orders')
        NUM_ORDERS = num.to_i
    end
end

alpha = %w{a b c d e f g h i j k l m n o p q r s t u v w x y z}
categories = Array.new
users = Array.new
orders = Array.new

NUM_CATEGORIES.times do
    length = rand(10) + 1
    cat = String.new
    length.times { cat << alpha[rand(25)] }
    
    categories.push(cat)
end

1.upto(NUM_USERS) do |id|
    name = String.new
    address = String.new
    state = String.new
    zip = rand(89999) + 10000
    random = rand * rand(1000)
    random = rand * rand(1000) while random < 0.01
    credit = random.to_s
    credit = credit[0..(credit.index('.') + 2)]

    length = rand(10) + 1
    length.times { name << alpha[rand(25)] }
    length = rand(90) + 10
    length.times { address << alpha[rand(25)] }
    length = rand(10) + 5
    length.times { state << alpha[rand(25)] }

    users.push(User.new(name, address, state, zip, credit, id))
end

NUM_ORDERS.times do
    title = String.new
    length = rand(20) + 1
    length.times { title << alpha[rand(25)] }

    random = rand * rand(100)
    random = rand * rand(100) while random < 0.01
    price = random.to_s
    price = price[0..(price.index('.') + 2)]
    category = categories[rand(categories.size)]
    id = users[rand(users.size)].id

    orders.push(Order.new(title, price, category, id))
end

File.open(ROOT + '/rand_test/db.txt', 'wb') { |f| users.each { |user| f << "\"#{user.name}\"|#{user.id}|#{user.credit}|\"#{user.address}\"|\"#{user.state}\"|\"#{user.zip}\"\n" } }
File.open(ROOT + '/rand_test/orders.txt', 'wb') { |f| orders.each { |order| f << "\"#{order.title}\"|#{order.price}|#{order.id}|#{order.category}\n" } }
File.open(ROOT + '/rand_test/cat.txt', 'wb') { |f| categories.each { |cat| f << cat + "\n" } }
