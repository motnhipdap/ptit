import orderModel from "../models/orderModel.js"
import userModel from "../models/userModel.js"
import Stripe from 'stripe'

const stripe = Stripe(process.env.STRIPE_SECRET_KEY)

const placeOrder = async (req, res) => {
    try {
        const { userId, items, amount, address } = req.body
        const orderData = {
            userId,
            items,
            amount,
            address,
            paymentMethod: "COD",
            payment: false,
            date: Date.now()
        }
        const newOrder = new orderModel(orderData)
        await newOrder.save()

        await userModel.findByIdAndUpdate(userId, { cartData: {} })

        res.json({ success: true, message: "Order Placed" })
    }
    catch (error) {
        console.log(error)
        res.json({ success: false, message: error.message })
    }
}

// Placing orders using Stripe Checkout
const placeOrderStripe = async (req, res) => {
    try {
        const { userId, items, amount, address } = req.body
        const { origin } = req.headers

        const orderData = {
            userId,
            items,
            amount,
            address,
            paymentMethod: "Stripe",
            payment: false,
            date: Date.now()
        }
        const newOrder = new orderModel(orderData)
        await newOrder.save()

        const lineItems = items.map((item) => ({
            price_data: {
                currency: 'inr',
                product_data: {
                    name: item.name,
                    images: item.images
                },
                unit_amount: item.price * 100,
            },
            quantity: item.quantity,
        }))

        const session = await stripe.checkout.sessions.create({
            payment_method_types: ['card'],
            line_items: lineItems,
            mode: 'payment',
            success_url: `${origin}/orders`,
            cancel_url: `${origin}/cart`,
            metadata: {
                orderId: newOrder._id.toString()
            }
        })

        res.json({ success: true, url: session.url })
    } catch (error) {
        console.log(error)
        res.json({ success: false, message: error.message })
    }
}

// Placing orders using Razorpay method
const placeOrderRazorpay = async (req, res) => {

}

// All orders data for admin panel
const allOrders = async (req, res) => {
    try {
        const orders = await orderModel.find({})
        res.json({ success: true, orders })
    }
    catch (error) {
        console.log(error)
        res.json({ success: false, message: error.message })
    }
}

// User order data for frontend
const userOrders = async (req, res) => {
    try {
        const { userId } = req.body
        const orders = await orderModel.find({ userId })
        res.json({ success: true, orders })
    }
    catch (error) {
        console.log(error)
        res.json({ success: false, message: error.message })
    }
}

// Update order status
const updateStatus = async (req, res) => {
    try {
        const { orderId, status } = req.body
        await orderModel.findByIdAndUpdate(orderId, { status })
        res.json({ success: true, message: "Order Status Updated" })
    }
    catch (error) {
        console.log(error)
        res.json({ success: false, message: error.message })
    }
}

export { placeOrder, placeOrderRazorpay, placeOrderStripe, allOrders, updateStatus, userOrders }