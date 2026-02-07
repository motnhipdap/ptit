import Ticket from "../models/ticket.js";

export const createTicket = async (req, res) => {
  try {
    const { productName, orderId, message } = req.body;
    const image = req.file ? req.file.path : null;

    if (!productName || !orderId || !image || !message) {
      return res.status(400).json({ message: "All fields required" });
    }

    const ticket = new Ticket({
      productName,
      orderId,
      image,
      message: [
        {
          sender: "user",
          text: message,
        },
      ],
    });

    await ticket.save();
    res.status(201).json({ message: "Ticket created", ticket });
  } catch (err) {
    res.status(500).json({ message: err.message });
  }
};

export const replyToTicket = async (req, res) => {
  try {
    const { message } = req.body;

    if (!message) {
      return res.status(400).json({ message: "Message required" });
    }
    const ticket = await Ticket.findById(req.params.id);
    if (!ticket) {
      return res.status(404).json({ message: "Ticket not found" });
    }

    ticket.message.push({
      sender: "admin",
      text: message,
    });

    ticket.status = "open";
    await ticket.save();

    res.json({ message: "Reply sent", ticket });
  } catch (err) {
    res.status(500).json({ message: err.message });
  }
};

export const getAllTickets = async (req, res) => {
  try {
    const tickets = await Ticket.find().sort({ createdAt: -1 });
    res.json(tickets);
  } catch (err) {
    res.status(500).json({ message: err.message });
  }
};

export const getTicketById = async (req, res) => {
  try {
    const ticket = await Ticket.findById(req.params.id);
    if (!ticket) {
      return res.status(404).json({ message: "Ticket not found" });
    }
    res.json(ticket);
  } catch (err) {
    res.status(500).json({ message: err.message });
  }
};

export const closeTicket = async (req, res) => {
  try {
    const ticket = await Ticket.findById(req.params.id);
    if (!ticket) {
      return res.status(404).json({ message: "Ticket not found" });
    }

    ticket.status = "closed";
    await ticket.save();

    res.json({ message: "Ticket closed successfully", ticket });
  } catch (err) {
    res.status(500).json({ message: err.message });
  }
};

