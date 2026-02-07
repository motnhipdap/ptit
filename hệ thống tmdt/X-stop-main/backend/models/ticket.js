import mongoose from 'mongoose';

const messageSchema=new mongoose.Schema({
    sender:{
        type:String,
        enum:["user","admin"],
        required:true,
    },
    text:{
        type:String,
        required:true,
    },
    createdAt:{
        type:Date,
        default:Date.now,
    }
})

const ticketSchema=new mongoose.Schema({
    productName:{
        type:String,
        required:true,
    },
    orderId:{
        type:Number,
        required:true
    },
    image:String,
    message: [messageSchema],
    status:{
        type:String,
        enum:["open","closed"],
        default:"open"
    }
});

export default mongoose.model("Ticket", ticketSchema);